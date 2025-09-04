#include "fm33lc0xx_fl.h"
#include "AT24CXXDataLoader.h"
#include "Define.h"
#include "TaskGpioInit.h"
#include "LPUart_0_And_1_Lib.h"
#include "TaskAllReceive.h"
#include "event_groups.h"
#include "Display.h"
#include "NumberBaseLib.h"
#include "PowerCtrl.h"
#include "MotorCtrlDev.h"
#include "TaskNBAndWeb.h"
#include "RTC_SetTime.h"

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

    /* PC4 EXTI10 */
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    GPIO_InitStruct.pin = FL_GPIO_PIN_4;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    EXTI_InitStruct.triggerEdge = FL_GPIO_EXTI_TRIGGER_EDGE_FALLING;
    EXTI_InitStruct.filter = FL_ENABLE;
    EXTI_InitStruct.input = FL_GPIO_EXTI_INPUT_GROUP0;
    GPIO_SET_H(GPIOC, FL_GPIO_PIN_4);

    FL_EXTI_Init(FL_GPIO_EXTI_LINE_9, &EXTI_InitStruct);
    FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_9);

    /* PC11 EXTI10 */
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    GPIO_InitStruct.pin = FL_GPIO_PIN_11;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    EXTI_InitStruct.triggerEdge = FL_GPIO_EXTI_TRIGGER_EDGE_FALLING;
    EXTI_InitStruct.filter = FL_ENABLE;
    EXTI_InitStruct.input = FL_GPIO_EXTI_INPUT_GROUP3;
    GPIO_SET_H(GPIOC, FL_GPIO_PIN_11);

    FL_EXTI_Init(FL_GPIO_EXTI_LINE_10, &EXTI_InitStruct);
    FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_10);
}
bool Check_Key_Line_value(void) {
    NowKeyData.KeyNum = 0;
    IncludeDelayMs(10);
    if (ReadKeyValue1 == 0) {
        NowKeyData.KeyNum = NowKeyData.KeyNum * 10 + 1;
    }
    IncludeDelayMs(30);
    KEY1_LOW;
    if (ReadKeyValue2 == 0) {
        NowKeyData.KeyNum = NowKeyData.KeyNum * 10 + 2;
    }
    KEY2_LOW;
    return false;
}
//////////////////////////////////////////////////////////////
EventGroupHandle_t xEventGroup_Gpio = NULL;
#define ShortTouchKey      (1 << 0) // 单击事件
#define LongTouchKey       (1 << 1) // 长按事件
#define NB_SendEvent       (1 << 2) // 长按事件
#define AllxEvent_Gpio (ShortTouchKey | LongTouchKey | NB_SendEvent)
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
    if ((NowKeyData.KeyNum == 2) && (xEventGroupGetBits(xEventGroup_Gpio) & LongTouchKey)) {
        xEventGroupClearBits(xEventGroup_Gpio, ShortTouchKey);  // 清除事件标志位
        xEventGroupClearBits(xEventGroup_Gpio, LongTouchKey);   // 清除事件标志位
        xEventGroupSetBits(xEventGroup_Gpio, NB_SendEvent);
        xSemaphoreGive(GpioTaskSemaphore);
    } else if ((readDataBit(RunKeyBitFlag, 0) || readDataBit(RunKeyBitFlag, 1)) || (xEventGroupGetBits(xEventGroup_Gpio) & LongTouchKey)) {
        // 按下 松开 长按====>激活事件
        EventBits_t uxBits = xEventGroupGetBits(xEventGroup_Gpio);
        // 任意一个事件发生则释放信号量
        if ((uxBits & AllxEvent_Gpio) != 0) {
            xSemaphoreGive(GpioTaskSemaphore);
        }
    }
    if (!NowKeyData.isInput) {
        CallCount = 0;
        if (xTimerStop(GpioTaskTimer, pdMS_TO_TICKS(5)) != pdPASS) {
            xTimerStop(GpioTaskTimer, portMAX_DELAY);
        }
        SetIsEnterLow_Power(true, 1); // 低功耗
    }
}

#include "TaskAboutTimer.h"
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
    while (xSemaphoreTake(GpioTaskSemaphore, portMAX_DELAY) == pdTRUE) {
        // NB 上传任务
        if (xEventGroupGetBits(xEventGroup_Gpio) & NB_SendEvent) {
            xEventGroupClearBits(xEventGroup_Gpio, NB_SendEvent);  // 清除事件标志位
            ShowWait(false);
            StartNBMode(false);
            continue;
        }
        // 等待信号量阻塞任务, 转为运行其他任务
        RTC_TASK.InitSetTimeTask(HomePageRefresh, 30, ShowHomePage);
        ////////////////////////////////***测量***////////////////////////////////
        if (xEventGroupGetBits(xEventGroup_Gpio) & ShortTouchKey) {
            xEventGroupClearBits(xEventGroup_Gpio, ShortTouchKey);  // 清除事件标志位
            LCD_MenuPage_Change(0);
        }
        if (xEventGroupGetBits(xEventGroup_Gpio) & LongTouchKey) {
            xEventGroupClearBits(xEventGroup_Gpio, LongTouchKey);   // 清除事件标志位
            if (((++KeyCountNum) % 5) == 0) {
                LCD_MenuPage_Change(0);
                OpenUartOfLightOrM1(); // 开启 M1 和 红外
            }
        }
        if (System_RunData.Now_Degree_Part == DegreePartToUint(100)) {
            setLED_Or_CtrlLinght(false, true, true, 3);
        } else if (System_RunData.Now_Degree_Part == DegreePartToUint(0)) {
            setLED_Or_CtrlLinght(true, false, true, 3);
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void GPIO_IRQHandler(void) {
    if (FL_GPIO_IsActiveFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_9)) { // 按键
        FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_9);
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTimerStartFromISR(GpioTaskTimer, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        SetIsEnterLow_Power(false, 1); // 暂时屏蔽低功耗
    }
    if (FL_GPIO_IsActiveFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_10)) { // 按键
        FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_10);
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTimerStartFromISR(GpioTaskTimer, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        SetIsEnterLow_Power(false, 1); // 暂时屏蔽低功耗
    }
}


