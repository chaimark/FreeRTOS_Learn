#include "fm33lc0xx_fl.h"
#include "../Include/Define.h"
#include "TaskGpioInit.h"
#include "LPUart_0_And_1_Lib.h"
#include "TaskAllReceive.h"
#include "event_groups.h"
#include "Display.h"
#include "NumberBaseLib.h"
UserKeyData NowKeyData;

/**
  * @brief  EXTI Initialization function
  * @param  void
  * @retval None
  */
void MF_EXTI_Init(void) {
    FL_EXTI_CommonInitTypeDef    EXTI_InitStruct;
    EXTI_InitStruct.clockSource = FL_RCC_EXTI_CLK_SOURCE_LSCLK;
    FL_EXTI_CommonInit(&EXTI_InitStruct);
}

/**
  * @brief  EXTI_Line9 Initialization function
  * @param  void
  * @retval None
  */
void MF_EXTI_Line_Init(void) {
    FL_GPIO_InitTypeDef     GPIO_InitStruct;
    FL_EXTI_InitTypeDef     EXTI_InitStruct;
    MF_EXTI_Init();

    /* PC10 EXTI10 */
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    GPIO_InitStruct.pin = FL_GPIO_PIN_10;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    EXTI_InitStruct.triggerEdge = FL_GPIO_EXTI_TRIGGER_EDGE_FALLING;
    EXTI_InitStruct.filter = FL_ENABLE;
    EXTI_InitStruct.input = FL_GPIO_EXTI_INPUT_GROUP2;
    GPIO_SET_H(GPIOC, FL_GPIO_PIN_10);

    FL_EXTI_Init(FL_GPIO_EXTI_LINE_10, &EXTI_InitStruct);
    FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_10);
}
bool Check_Key_Line_value(void) {
    IncludeDelayMs(10);
    if (ReadKeyValue == 0) {
        // NowKeyData.KeyNum = setDataBit(NowKeyData.KeyNum, No1_key, true);
        return true;
    }
    IncludeDelayMs(30);
    return false;
}
//////////////////////////////////////////////////////////////
EventGroupHandle_t xEventGroup_Gpio = NULL;
#define ShortTouchKey      (1 << 0) // 单击事件
#define LongTouchKey       (1 << 1) // 长按事件.
#define AllxEvent_Gpio (ShortTouchKey | LongTouchKey)
//////////////////////////////////////////////////////////////
static TimerHandle_t GpioTaskTimer = NULL;
TaskHandle_t GpioTaskHand = NULL;
SemaphoreHandle_t GpioTaskSemaphore = NULL;
void GpioTaskTimerCallback(TimerHandle_t xTimer) {
#define DownKey_Run
// #define UpKey_Run
    uint8_t RunKeyBitFlag = 0;
    static uint16_t CallCount = 0;
    CallCount++;
    if (CallCount <= 10) {
        xEventGroupSetBits(xEventGroup_Gpio, ShortTouchKey);   // 属于点按
    } else {
        xEventGroupSetBits(xEventGroup_Gpio, LongTouchKey);    // 属于长按
    }
    NowKeyData.isInput = Check_Key_Line_value();  // 检测是否按下按钮
// 按下触发
#ifdef DownKey_Run
    if (NowKeyData.isInput && CallCount == 1) {
        RunKeyBitFlag = (uint8_t)setDataBit(RunKeyBitFlag, 0, true);
    }
#endif
// 松开触发
#ifdef UpKey_Run    
    if ((!NowKeyData.isInput) && CallCount < 10) {
        RunKeyBitFlag = (uint8_t)setDataBit(RunKeyBitFlag, 1, true);
    }
#endif
    // 按下 松开 长按====>激活事件
    if ((readDataBit(RunKeyBitFlag, 0) || readDataBit(RunKeyBitFlag, 1)) || (xEventGroupGetBits(xEventGroup_Gpio) & LongTouchKey)) {
        // 任意一个事件发生则释放信号量
        EventBits_t uxBits = xEventGroupGetBits(xEventGroup_Gpio);
        if ((uxBits & AllxEvent_Gpio) != 0) {
            xSemaphoreGive(GpioTaskSemaphore);
        }
    }
    if (!NowKeyData.isInput) {
        CallCount = 0;
        xTimerStop(GpioTaskTimer, 0); // 松开立即停止定时器
        return;
    } else if ((xEventGroupGetBits(xEventGroup_Gpio) & LongTouchKey)) { // 长按状态需要暂时关闭定时器
        xTimerStop(GpioTaskTimer, 0); // 松开立即停止定时器
    }
}

void GpioTask(void * pvParameters) {
    static uint16_t KeyCountNum = 0;
    GpioTaskTimer = xTimerCreate(
        "GpioTask_Time",          // 定时器名称
        pdMS_TO_TICKS(50),        // 定时周期
        pdTRUE,                   // 自动重载
        (void *)0,                // 定时器 ID
        GpioTaskTimerCallback     // 回调函数
    );
    // 创建事件组
    xEventGroup_Gpio = xEventGroupCreate();
    if (xEventGroup_Gpio == NULL) {
        Reboot.EC20_ReBoot_In_RTC_By_1Second(&Reboot, 10);
    }
    GpioTaskSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    while (1) {
        // 等待信号量阻塞任务, 转为运行其他任务
        if (xSemaphoreTake(GpioTaskSemaphore, portMAX_DELAY) == pdTRUE) {
            RTC_TASK.InitSetTimeTask(HomePageRefresh, 60, ShowHomePage);
            OpenUartOfLightOrM1(); // 开启 M1 和 红外
            ////////////////////////////////***测量***////////////////////////////////
            if (xEventGroupGetBits(xEventGroup_Gpio) & ShortTouchKey) {
                xEventGroupClearBits(xEventGroup_Gpio, ShortTouchKey);  // 清除事件标志位
                LCD_MenuPage_Change();
            }
            if (xEventGroupGetBits(xEventGroup_Gpio) & LongTouchKey) {
                xEventGroupClearBits(xEventGroup_Gpio, LongTouchKey);   // 清除事件标志位
                if (((++KeyCountNum) % 5) == 0) {
                    LCD_MenuPage_Change();
                }
                xTimerStart(GpioTaskTimer, 0);  // 长按重启定时器
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void GPIO_IRQHandler(void) {
    if (FL_GPIO_IsActiveFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_10)) { // 按键
        FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_10);
        if (GpioTaskTimer != NULL) {
            xTimerStartFromISR(GpioTaskTimer, 0);  // 启动定时器
        }
    }
    if (FL_GPIO_IsActiveFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_12)) { // GD0
        FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_12);
        RF_ReciveData();
    }
}

