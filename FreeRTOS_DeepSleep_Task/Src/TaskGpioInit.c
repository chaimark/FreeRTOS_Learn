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
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
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
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
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

    FL_NVIC_ConfigTypeDef   InterruptConfigStruct;
    InterruptConfigStruct.preemptPriority = 0x000B;
    FL_NVIC_Init(&InterruptConfigStruct, GPIO_IRQn);
}
bool Check_Key_Line_value(void) {
    bool ResFlag = false;
    NowKeyData.KeyNum = 0;
    IncludeDelayMs(10);
    if (ReadKeyValue1 == 0) {
        NowKeyData.KeyNum = NowKeyData.KeyNum * 10 + 1;
        ResFlag = true;
    }
    IncludeDelayMs(30);
    KEY1_LOW;
    if (ReadKeyValue2 == 0) {
        NowKeyData.KeyNum = NowKeyData.KeyNum * 10 + 2;
        ResFlag = true;
    }
    KEY2_LOW;
    return ResFlag;
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
/* 软件定时器回调，跑在任务上下文 */
void GpioTaskTimerCallback(TimerHandle_t xTimer) {
#define DownKey_Run
// #define UpKey_Run
    uint8_t RunKeyBitFlag = 0;
    static uint16_t CallCount = 0;
    CallCount++;
    NowKeyData.isInput = Check_Key_Line_value();
#ifdef DownKey_Run
    if (NowKeyData.isInput && CallCount == 1) {
        RunKeyBitFlag = (uint8_t)setDataBit(RunKeyBitFlag, 0, true);
    }
#endif
#ifdef UpKey_Run
    if (!NowKeyData.isInput) {
        RunKeyBitFlag = (uint8_t)setDataBit(RunKeyBitFlag, 1, true);
    }
#endif
    // 首次触发
    if (readDataBit(RunKeyBitFlag, 0) || readDataBit(RunKeyBitFlag, 1)) {
        if ((CallCount <= 10) && (NowKeyData.KeyNum != 2)) {    // 按键 2 没有点击功能
            xEventGroupSetBits(xEventGroup_Gpio, ShortTouchKey);
        }
    } else if ((CallCount > 10)&&(NowKeyData.isInput)) {    // 长按操作
        xEventGroupSetBits(xEventGroup_Gpio, LongTouchKey);
        if (NowKeyData.KeyNum == 2) {
            CallCount = 0;
            SetIsEnterLow_Power(true, 1);
        }
        return;
    }
    if (!NowKeyData.isInput) {
        CallCount = 0;
        SetIsEnterLow_Power(true, 1);
    } else {
        xTimerStart(xTimer, 0);
    }
}
#include "TaskAboutTimer.h"
void GpioTask(void * pvParameters) {
    static uint16_t KeyCountNum = 0;
    GpioTaskTimer = xTimerCreate(
        "GpioTask_Time",          // 定时器名称
        pdMS_TO_TICKS(30),        // 定时周期
        pdFALSE,                  // 不自动重载
        (void *)0,                // 定时器 ID
        GpioTaskTimerCallback     // 回调函数
    );
    // 创建事件组
    xEventGroup_Gpio = xEventGroupCreate();
    if (xEventGroup_Gpio == NULL) {
        Reboot.EC20_ReBoot_In_RTC_By_1Second(&Reboot, 10);
    }
    while (1) {
        // printf("%s left:%u\n", pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));
        /* 永久等待任意事件位 */
        EventBits_t uxBits = xEventGroupWaitBits(
            xEventGroup_Gpio,
            AllxEvent_Gpio,          // OtherValve | NBCtrlValve
            pdTRUE,                   // 退出时自动清除
            pdFALSE,                  // 任意一位就唤醒
            portMAX_DELAY
        );
        if (System_RunData.isPowerModuleUseing == true) {
            NowKeyData.isInput = false;
            NowKeyData.KeyNum = 0;
            continue;
        }
        // NB 上传任务
        if (uxBits & NB_SendEvent) {
            NowKeyData.isInput = false;
            NowKeyData.KeyNum = 0;
            _Module_Start_Flage = _Module_UserSend;
            SetIsEnterLow_Power(true, 1);
            continue;
        }
        // 等待信号量阻塞任务, 转为运行其他任务
        RTC_TASK.InitSetTimeTask(HomePageRefresh, 30, ShowHomePage);
        ////////////////////////////////*** 长短事件 ***////////////////////////////////
        if ((uxBits & ShortTouchKey) && (NowKeyData.KeyNum != 2)) {
            LCD_MenuPage_Change(0);
        }
        if (uxBits & LongTouchKey) {
            if ((NowKeyData.KeyNum == 2) && (readDataBit(AT24CXX_Manager_NET.ModeCode, Enable_NBMode))) {
                xEventGroupSetBits(xEventGroup_Gpio, NB_SendEvent);
            } else {
                if (((++KeyCountNum) % 3) == 0) {
                    LCD_MenuPage_Change(0);
                    OpenUartOfLightOrM1(); // 开启 M1 和 红外
                }
                xTimerStart(GpioTaskTimer, 0);
            }
        }
        ////////////////////// 触发按键后刷新 LED 并初始化按键 //////////////////////////
        if (System_RunData.Now_Degree_Part == DegreePartToUint(100)) {
            setLED_Or_CtrlLinght(false, true, true, 3);
        } else if (System_RunData.Now_Degree_Part == DegreePartToUint(0)) {
            setLED_Or_CtrlLinght(true, false, true, 3);
        }
        NowKeyData.isInput = false;
        NowKeyData.KeyNum = 0;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void GPIO_IRQHandler(void) {
    BaseType_t xHPW_TaskWoken = pdFALSE;
    if (FL_GPIO_IsActiveFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_9)) { // 按键
        FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_9);
        if (GpioTaskTimer == NULL) {
            return;
        }
        xTimerStartFromISR(GpioTaskTimer, &xHPW_TaskWoken);
        SetIsEnterLow_Power(false, 1); // 暂时屏蔽低功耗
    }
    if (FL_GPIO_IsActiveFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_10)) { // 按键
        FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_10);
        if (GpioTaskTimer == NULL) {
            return;
        }
        xTimerStartFromISR(GpioTaskTimer, &xHPW_TaskWoken);
        SetIsEnterLow_Power(false, 1); // 暂时屏蔽低功耗
    }
    portYIELD_FROM_ISR(xHPW_TaskWoken);
}


