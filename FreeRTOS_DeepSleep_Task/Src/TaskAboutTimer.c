#include "TaskAboutTimer.h"
#include "RTC_SetTime.h"
#include "AT24CXXDataLoader.h"
#include "StrLib.h"
#include "ADC.h"
#include "timers.h"
#include "event_groups.h"
#include "Define.h"
#include "Task_Valve.h"
#include "NumberBaseLib.h"
#include "LPUart_0_And_1_Lib.h"
#include "Display.h"
#include "PowerCtrl.h"
#include "NB_Mode_Lib.h"

EventGroupHandle_t xEventGroup_TimeTask = NULL;
#define xEvent_TestBatV             (1 << 0)
#define xEvent_TestTempe            (1 << 1)
#define xEvent_TestValve            (1 << 2)
#define xEvent_yuliu1               (1 << 3)
#define xEvent_checkTimeLimit       (1 << 4)
#define xEvent_yuliu2               (1 << 5)
#define xEvent_ValveAutoTest        (1 << 6)
#define xEvent_yuliu3               (1 << 7)
#define xEvent_yuliu4               (1 << 8)
#define xEvent_yuliu5               (1 << 9) // 这个 20 分钟
#define xEvent_TestAlarm            (1 << 10)
#define AllxEvent_Timer (xEvent_TestBatV | xEvent_TestTempe | xEvent_TestValve | xEvent_yuliu1 | xEvent_checkTimeLimit | xEvent_yuliu2 | xEvent_ValveAutoTest | xEvent_yuliu3 | xEvent_yuliu4 | xEvent_yuliu5 | xEvent_TestAlarm)
TaskHandle_t TimeTaskHand = NULL;
SemaphoreHandle_t TimeTaskSemaphore = NULL;
// 设置 LED 点亮或关闭，以及对应状态的持续时间
void setLED_Or_CtrlLinght(bool CloseLEDStart, bool OpenLEDStart, bool isSetLED, int8_t light_S) {
    static uint8_t CloseLEDRunTime = 0;
    static uint8_t OpenLEDRunTime = 0;
    static bool FrontCloseLEDStart = false;
    static bool FrontOpenLEDStart = false;
    if (isSetLED) {
        CloseLEDRunTime = light_S;
        OpenLEDRunTime = light_S;
        FrontCloseLEDStart = CloseLEDStart;
        FrontOpenLEDStart = OpenLEDStart;
#if(configUSE_TICKLESS_IDLE == 1)
        if (CloseLEDStart == true) {
            LED_Close_HIGH;
        } else if (CloseLEDStart == false) {
            LED_Close_LOW;
        }
        if (OpenLEDStart == true) {
            LED_Open_HIGH;
        } else if (OpenLEDStart == false) {
            LED_Open_LOW;
        }
#endif
    } else {
        if ((CloseLEDRunTime == 0) || (OpenLEDRunTime == 0)) {
            return;
        }
        CloseLEDRunTime = (CloseLEDRunTime > 0 ? CloseLEDRunTime - 1 : 0);
        OpenLEDRunTime = (OpenLEDRunTime > 0 ? OpenLEDRunTime - 1 : 0);
        if (CloseLEDRunTime == 0) {
            FrontCloseLEDStart = false;
        }
        if (OpenLEDRunTime == 0) {
            FrontOpenLEDStart = false;
        }
        if (FrontCloseLEDStart == true) {
            LED_Close_HIGH;
        } else if (FrontCloseLEDStart == false) {
            LED_Close_LOW;
        }
        if (FrontOpenLEDStart == true) {
            LED_Open_HIGH;
        } else if (FrontOpenLEDStart == false) {
            LED_Open_LOW;
        }
    }
}
#include "TaskNBAndWeb.h"
void startTimeTask(void) {
    BaseType_t xHPW_TaskWoken = pdFALSE;
    static uint32_t CallCount = 0;
    RTC_TASK.InitSetTimeTask(TaskAllTime, 1, startTimeTask);
    if (TimeTaskSemaphore == NULL) { // 唤醒
        return;
    }
    ShowSignal();
    setLED_Or_CtrlLinght(0xFF, 0xFF, false, -1); // 刷新LED
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, Enable_NBMode)) {
        if ((_Module_Start_Flage == 0) && (System_RunData.isPowerModuleUseing == false)) {
            if (System_RunData.Now_NetDevParameter.isWriteEEprom == true) {
                StartWriteAT24C0xxDataFromISR(&xHPW_TaskWoken);
            } else if (System_RunData.Now_NetDevParameter.isWriteEEprom == _Module_EEpromSend) {
                StartWriteAT24C0xxDataFromISR(&xHPW_TaskWoken);
                RTC_TASK.InitSetTimeTask(SendIntervalTask, MinToSec(15), NULL);
            } else if (System_RunData.Now_NetDevParameter.isWriteEEprom == _Module_EEpromTime) {
                StartWriteNowRTCTimeFromISR(&xHPW_TaskWoken);
            } else if (System_RunData.CtrlDev_Set_Degree_Part != 0xFFFF) {
                StartValveRunFromISR(&xHPW_TaskWoken);
            }
            setIsWriteEEprom(false);
        }

        if (Min_15_TASK.Task[AgainSendData].TimeTask_Falge) {  // 是否重发
            _Module_Start_Flage = _Module_AutoSend;
            Min_15_TASK.CloseTask(AgainSendData);
        }
        if ((RTC_TASK.Task[SendIntervalTask].TimeTask_Falge) && (System_RunData.Now_NetDevParameter.isWriteEEprom == false)) {   // _Module_ 发送判断机制
            _Module_Start_Flage = _Module_AutoSend;
            RTC_TASK.InitSetTimeTask(SendIntervalTask, AT24CXX_Manager_NET.SendManageObj.SendIntervalDay, NULL);
        }
        if ((_Module_Start_Flage >= _Module_UserSend) && (System_RunData.Now_NetDevParameter.NowNetOnlineFlag == false)) {
            StartNBModeFromISR(&xHPW_TaskWoken);
        }
    }
    if (CallCount % (5 * 60) == 0) { // 5 * 60 sec
        xEventGroupSetBitsFromISR(xEventGroup_TimeTask, xEvent_TestBatV, &xHPW_TaskWoken);      // 设置标志位, 测电压
    }
    if (CallCount % 30 == 0) { // 30 sec
        xEventGroupSetBitsFromISR(xEventGroup_TimeTask, xEvent_TestTempe, &xHPW_TaskWoken);     // 设置标志位, 测温度
    }
    if (CallCount % (1 * 60) == 0) { // 1 * 60 sec
        xEventGroupSetBitsFromISR(xEventGroup_TimeTask, xEvent_TestValve, &xHPW_TaskWoken);     // 设置标志位, 测阀门
    }
    if ((CallCount % (30 * 60) == 0) && (CallCount != 0)) {
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableTimeLimit)) {                       // 设置标志位，截止时间 
            xEventGroupSetBitsFromISR(xEventGroup_TimeTask, xEvent_checkTimeLimit, &xHPW_TaskWoken);
        }
    }
    if ((CallCount % (15 * 24 * 60 * 60) == 0) && (CallCount != 0)) {                           // 设置标志位，自检
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, ValveAutoTest)) {
            xEventGroupSetBitsFromISR(xEventGroup_TimeTask, xEvent_ValveAutoTest, &xHPW_TaskWoken);
        }
    }
    if (CallCount % 5 == 0) {
        xEventGroupSetBitsFromISR(xEventGroup_TimeTask, xEvent_TestAlarm, &xHPW_TaskWoken);     // 设置标志位, 检测是否存在报警信息
    }
    CallCount++;
    // 任意一个事件发生则释放信号量
    if ((System_RunData.isPowerModuleUseing != true) && (xEventGroupGetBitsFromISR(xEventGroup_TimeTask) & AllxEvent_Timer) != 0) {
        // 发出信号量, 准备继续运行
        xSemaphoreGiveFromISR(TimeTaskSemaphore, &xHPW_TaskWoken);
        if (xTaskResumeFromISR(TimeTaskHand) == pdTRUE) {
            xHPW_TaskWoken = pdTRUE;                 /* 标记需要切换 */
        }
    }
    portYIELD_FROM_ISR(xHPW_TaskWoken);          /* 统一在函数末尾切换 */
}
void TestValveAutoSub(void) {
    static uint16_t TempSave_Degree_Part = 0;
    TempSave_Degree_Part = System_RunData.Now_Degree_Part;
    SetValve TempSetdata = {
        .DoneId = VALVE_REASON_SELF_CHECK,
        .MaxDelay_1000Tick = 0,
        .Set_Degree_Part = 0,
    };
    TempSetdata.DoneId = VALVE_REASON_SELF_CHECK;
    TempSetdata.MaxDelay_1000Tick = 120;
    if (System_RunData.Now_Degree_Part < DegreePartToUint(30)) {
        TempSetdata.Set_Degree_Part = 0;                        // 复位到 0
        ValveCtrlStart(TempSetdata);
        //////////////////////////////////////////////////////
        TempSetdata.Set_Degree_Part = DegreePartToUint(100);
        ValveCtrlStart(TempSetdata);    // 0 开到 100 保存满开电压
        TempSetdata.Set_Degree_Part = 0;
        ValveCtrlStart(TempSetdata);    // 100 关到 0 保存全关电压
        TempSetdata.Set_Degree_Part = TempSave_Degree_Part;
        ValveCtrlStart(TempSetdata);    // 恢复原来的阀门状态
    } else {
        TempSetdata.Set_Degree_Part = DegreePartToUint(100);    // 复位到 100
        ValveCtrlStart(TempSetdata);
        //////////////////////////////////////////////////////
        TempSetdata.Set_Degree_Part = 0;
        ValveCtrlStart(TempSetdata);    // 100 关到 0 保存全关电压
        TempSetdata.Set_Degree_Part = DegreePartToUint(100);
        ValveCtrlStart(TempSetdata);    // 0 开到 100 保存满开电压
        TempSetdata.Set_Degree_Part = TempSave_Degree_Part;
        ValveCtrlStart(TempSetdata);    // 恢复原来的阀门状态
    }
}
/*
    D 温控器  被拆卸报警
    E 温控器  无线失联超2小时
    F 温控器  低电报警
    G 阀门低电报警
    H 刷卡故障
    I  无线故障
*/
void TestSystemAlarm(void) {
    if (System_RunData.Now_DEV_Volt < 3200) {
        SetErrerCode(CODE_ERR_VALVE_LOW_BATTERY, true);
    } else {
        SetErrerCode(CODE_ERR_VALVE_LOW_BATTERY, false);
    }
}
void startRunTask(TimerHandle_t xTimer) {
    Test_ValueDegree(0, true); // 测试阀门度数, 单次测量
    System_RunData.FrontValveDegree_Part = System_RunData.Now_Degree_Part;
    if (System_RunData.Now_Degree_Part == DegreePartToUint(0) || System_RunData.Now_Degree_Part == DegreePartToUint(100)) {
        AT24CXX_Manager_NET.UserSet_DegreePart = System_RunData.Now_Degree_Part;
    }
    // 创建硬件定时器
    RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
    RTC_TASK.InitSetTimeTask(TaskAllTime, 1, startTimeTask);
    Min_15_TASK.InitSetTimeTask(DayOverCclkTask, HourToMin_15(AT24CXX_Manager_NET.DaysNumberOfCCLK * 24), NULL);
    InitSendModeAndTimeTask();  // 复位时, 重新计算当天的发送模式和时间任务
    SetIsEnterLow_Power(true, 0); // 低功耗
}
void TimeTask(void * pvParameters) {
    SetIsEnterLow_Power(false, 0); // 暂时屏蔽低功耗
    TimerHandle_t StartSend433Timer = xTimerCreate(
        "StartSend433",         // 定时器名称
        pdMS_TO_TICKS(200),     // 定时周期 1000ms
        pdFALSE,                // 不自动重载
        (void *)0,              // 定时器 ID
        startRunTask            // 回调函数
    );
    xTimerStart(StartSend433Timer, pdMS_TO_TICKS(5)); // 开启 433 首发计时器
    IncludeDelayMs(1000);
    xTimerDelete(StartSend433Timer, pdMS_TO_TICKS(5));
    TimeTaskSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    // 创建事件组
    xEventGroup_TimeTask = xEventGroupCreate();
    if (xEventGroup_TimeTask == NULL) {
        Reboot.EC20_ReBoot_In_RTC_By_1Second(&Reboot, 10);
    }
    do {
        // printf("%s left:%u\n", pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));
        ////////////////////////////////***测量***////////////////////////////////
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_TestBatV) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_TestBatV);       // 清除事件标志位
            Test_BatVoltge();
        }
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_TestTempe) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_TestTempe);      // 清除事件标志位
            Test_Temperature();
        }
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_TestValve) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_TestValve);      // 清除事件标志位
            if (System_RunData.Now_Degree_Part > DegreePartToUint(40)) {
                System_RunData.TempRunTimeForValve++;
            }
            Test_ValueDegree(0, true); // 测试阀门度数, 单次测量
        }
        ////////////////////////////////***阀门***////////////////////////////////
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_checkTimeLimit) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_checkTimeLimit);
            uint32_t MaxTime = getTimeNumber_UTCByRTCTime(NEW_NAME(AT24CXX_Manager_NET.LimitTime));
            newString(NowTimeStr, 30);
            sprintf(NowTimeStr.Name._char, "20%02x-%02x-%02x %02x:%02x:%02x", NowYear, NowMonth, NowDay, NowHour, NowMinute, NowSecond);
            uint32_t NowTime = getTimeNumber_UTCByRTCTime(NowTimeStr);
            SetValve TempSetdata = {
                .DoneId = VALVE_REASON_SYSTEM_DEADLINE,
                .MaxDelay_1000Tick = 5,
                .Set_Degree_Part = 0xFFFF,
            };
            if (NowTime > MaxTime) {
                TempSetdata.Set_Degree_Part = 0;
            } else {
                if (System_RunData.Now_Degree_Part < DegreePartToUint(30)) {
                    TempSetdata.Set_Degree_Part = DegreePartToUint(100);
                }
            }
            if (TempSetdata.Set_Degree_Part != 0xFFFF) {
                ValveCtrlStart(TempSetdata);
            }
        }
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_ValveAutoTest) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_ValveAutoTest);
            TestValveAutoSub();
        }
        ////////////////////////////////***故障***////////////////////////////////
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_TestAlarm) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_TestAlarm);
            TestSystemAlarm();
        }
    } while (xSemaphoreTake(TimeTaskSemaphore, portMAX_DELAY) == pdTRUE);   // 等待信号量阻塞任务, 转为运行其他任务
}
void StartValveAutoTest(void) {
    xEventGroupSetBits(xEventGroup_TimeTask, xEvent_ValveAutoTest);
}
void StartLimitCheckTask(void) {
    xEventGroupSetBits(xEventGroup_TimeTask, xEvent_checkTimeLimit);
}

