#include "TaskAllReceive.h"
#include "LPUart_0_And_1_Lib.h"
#include "RTC_SetTime.h"
#include "AT24CXXDataLoader.h"
#include "PublicLib_No_One.h"
#include "StrLib.h"
#include "Display.h"

TimerHandle_t CloseLightOrM1_TASK = NULL;
uint8_t getUartx = 0x00;
void OpenBSTime(void) {
    if (getUartx == 0x00) {
        BSTIM32_Setup();    //开启定时器中断
#ifdef OPEN_LOWPWER_DEBUG
        RTC_TASK.InitSetTimeTask(LowPwerDebug, 1, NULL);
#endif
        getUartx = 0xF0;
    }
}
TaskHandle_t AllReceive_Hand = NULL;
SemaphoreHandle_t LPUartxSemaphore = NULL;
void LPUart0CheckBuff(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // 向 TimeTask 发出信号量, 准备继续运行
    xSemaphoreGiveFromISR(LPUartxSemaphore, &xHigherPriorityTaskWoken);
    // 恢复 TimeTask 任务
    if (xTaskResumeFromISR(AllReceive_Hand) == pdFALSE) {
        // 如果需要切换上下文
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    getUartx |= 0x01; // 设置标志位
}
void RF_ReciveData(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // 向 TimeTask 发出信号量, 准备继续运行
    xSemaphoreGiveFromISR(LPUartxSemaphore, &xHigherPriorityTaskWoken);
    // 恢复 TimeTask 任务
    if (xTaskResumeFromISR(AllReceive_Hand) == pdFALSE) {
        // 如果需要切换上下文
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    getUartx |= 0x08; // 设置标志位
}
static uint8_t isReadCard = false;
void closeLightOrM1(TimerHandle_t xTimer) {
    static uint8_t CallCount = 0;
    if (CallCount == 0) {
        isReadCard = true;
    }
    CallCount++;
    if (CallCount == 30 * 2) {
        CallCount = 0;
        xTimerStop(CloseLightOrM1_TASK, 0); // 停止计时器
        Infrared_POWER_OFF;     // 关闭 红外
    }
    if (CallCount == 10 * 2) {
        M1_POWER_OFF;   // 关闭 M1
        isReadCard = false;
    }
    if (isReadCard == true) {
        xSemaphoreGive(LPUartxSemaphore);
        getUartx |= 0x04; // 设置标志位
    }
}
#include "Task_Valve.h"
#include "SI522A_App.h"
// 截止日期卡
void LimitCard(strnew SetLimitTime) {
    newString(NowTimeStr, 30);
    sprintf(NowTimeStr.Name._char, "20%02x-%02x-%02x %02x:%02x:%02x", NowYear, NowMonth, NowDay, NowHour, NowMinute, NowSecond);
    uint32_t NowTime = getTimeNumber_UTCByRTCTime(NowTimeStr);
    memset(NowTimeStr.Name._char, 0, NowTimeStr.MaxLen);
    sprintf(NowTimeStr.Name._char, "20%02x-%02x-%02x %02x:%02x:%02x",
        SetLimitTime.Name._char[0],
        SetLimitTime.Name._char[1],
        SetLimitTime.Name._char[2],
        SetLimitTime.Name._char[3],
        SetLimitTime.Name._char[4],
        SetLimitTime.Name._char[5]
    );
    uint32_t MaxTime = getTimeNumber_UTCByRTCTime(NowTimeStr);
    if (NowTime > MaxTime) {
        Get_Module_Data.CtrlDev_Set_Degree_Part = 0;
    } else {
        if (Get_Module_Data.Now_Degree_Part < 300) {
            Get_Module_Data.CtrlDev_Set_Degree_Part = 1000;
        }
        Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
    }
    if (Get_Module_Data.CtrlDev_Set_Degree_Part != 0xFFFF) {
        SetValve TempSetdata = {
            .DoneId = VALVE_REASON_SYSTEM_DEADLINE,
            .MaxDelay_1000Tick = 5,
            .Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part,
        };
        ValveCtrlStart(TempSetdata);
        Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
    }
}
// 角度设定卡
void CtrlValveCard(uint16_t SetDegree) {
    if (SetDegree > 1000) {
        Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
    }
    Get_Module_Data.CtrlDev_Set_Degree_Part = SetDegree;
}
// 校时卡
void SetRTCtimeCard(strnew SetRTCTime) {
    char setStrOfTime[32] = {0};
    strnew NowTimeData = NEW_NAME(setStrOfTime);
    sprintf(NowTimeData.Name._char, "20%02x-%02x-%02x %02x:%02x:%02x",
        SetRTCTime.Name._char[0],
        SetRTCTime.Name._char[1],
        SetRTCTime.Name._char[2],
        SetRTCTime.Name._char[3],
        SetRTCTime.Name._char[4],
        SetRTCTime.Name._char[5]
    );
    setRtcDate(NowTimeData, false); // 设置时间
}
bool DoneCardCmd(UserCard UserCardData) {
    if (!(UserCardData.MeterID[0] == 0xFF) && (UserCardData.MeterID[1] == 0xFF) && (UserCardData.MeterID[2] == 0xFF) && (UserCardData.MeterID[3] == 0xFF)) {
        if (!(AT24CXX_Manager_NET.MeterID[0] == UserCardData.MeterID[0]) &&
            (AT24CXX_Manager_NET.MeterID[1] == UserCardData.MeterID[1]) &&
            (AT24CXX_Manager_NET.MeterID[2] == UserCardData.MeterID[2]) &&
            (AT24CXX_Manager_NET.MeterID[3] == UserCardData.MeterID[3])) {
            return false;
        }
    }
    switch (UserCardData.CmdFlag) {
        case 0:
            LimitCard(NEW_NAME(UserCardData.SetLimitTime));
            break;
        case 1:
            CtrlValveCard(UserCardData.SetDegree);
            break;
        case 2:
            SetRTCtimeCard(NEW_NAME(UserCardData.SetRTCTime));
            break;
        default:
            return false;
    }
    return true;
}


void AllReceive(void * pvParameters) {
    CloseLightOrM1_TASK = xTimerCreate(
        "CloseUartTask",        // 定时器名称
        pdMS_TO_TICKS(500),     // 定时周期 500ms
        pdTRUE,                 // 不自动重载
        (void *)0,              // 定时器 ID
        closeLightOrM1          // 回调函数
    );
    getUartx = 0x00;
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, Enable433Open)) {
        Send_RF_DATA_AT_The_Beginning();
    } else {
        CMT2300A_RF_CLOSE();
    }
    LPUartxSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    SetValve TempSetdata = {
        .DoneId = VALVE_REASON_RETURN_WATER_TEMP,
        .MaxDelay_1000Tick = 0,
        .Set_Degree_Part = 0,
    };
    while (1) {
        if (xSemaphoreTake(LPUartxSemaphore, portMAX_DELAY) == pdTRUE) {
            if (getUartx & 0x01) {
                if (HY_USB_TTL_CheckBuff(LPUART0Ddata.RxBuf, LPUART0Ddata.RxLen, getUartx)) {
                    LPUART0_Send((unsigned char *)LPUART0Ddata.TxBuf, LPUART0Ddata.TxLen);
                }
                Clr_LPUart0_RxBuffer();
                SetTime.CloseTask(RecTimeLPUART0);
                BSTIM32_Stop();     //关闭定时器中断
                if (Get_Module_Data.CtrlDev_Set_Degree_Part != 0xFFFF) {
                    if (CloseLightOrM1_TASK != NULL && xTimerIsTimerActive(CloseLightOrM1_TASK) != pdFALSE) {
                        TempSetdata.DoneId = VALVE_REASON_INFRARED_INTERFACE;
                        TempSetdata.MaxDelay_1000Tick = 5;
                        TempSetdata.Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part;
                    } else {
                        TempSetdata.DoneId = VALVE_REASON_MBUS_INTERFACE;
                        TempSetdata.MaxDelay_1000Tick = 5;
                        TempSetdata.Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part;
                    }
                    ValveCtrlStart(TempSetdata);
                    Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
                }
                getUartx &= ~0x01;
            }
            if (getUartx & 0x04) {
                getUartx &= ~0x04;
                if (!PCD_SI522A_TypeA_rw_block(DoneCardCmd)) {
                    continue; // 没有刷卡，跳过
                }
                isReadCard = false;
                if (Get_Module_Data.CtrlDev_Set_Degree_Part != 0xFFFF) {
                    TempSetdata.DoneId = VALVE_REASON_CARD_SWIPE;
                    TempSetdata.MaxDelay_1000Tick = 5;
                    TempSetdata.Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part;
                    ValveCtrlStart(TempSetdata);
                    Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
                }
            }
            if (getUartx & 0x08) {
                if (isReceiveData()) {
                    ShowSignal(true);
                    Module_Send_Data();
                    vTaskDelay(500);
                    ShowSignal(false);
                }
                getUartx &= ~0x08;
                if (!readDataBit(AT24CXX_Manager_NET.ModeCode, Enable433Open)) {
                    CMT2300A_RF_CLOSE();
                } else if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableUserCtrlValve)) {
                    if (readDataBit(AT24CXX_Manager_NET.ModeCode, UserCtrlAbleHeatRange)) {
                        if (Get_Module_Data.CtrlDev_TemperRoom < AT24CXX_Manager_NET.InsetT_Min * 10) {
                            Get_Module_Data.CtrlDev_Set_Degree_Part = 1000;
                        } else if (Get_Module_Data.CtrlDev_TemperRoom > AT24CXX_Manager_NET.InsetT_Max * 10) {
                            Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
                        }
                    }
                    if (Get_Module_Data.CtrlDev_Set_Degree_Part != 0xFFFF) {
                        TempSetdata.DoneId = VALVE_REASON_WIRELESS_THERMOSTAT;
                        TempSetdata.MaxDelay_1000Tick = 5;
                        TempSetdata.Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part;
                        ValveCtrlStart(TempSetdata);
                        Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
                    }
                }
            }
            getUartx = 0x00;
        }
    }
}
/////////////////////////////////////////////
void OpenUartOfLightOrM1(void) {
    // 判断定时器是否已经启动
    if (CloseLightOrM1_TASK != NULL && xTimerIsTimerActive(CloseLightOrM1_TASK) != pdFALSE) {
        return; // 定时器已启动，无需重复启动
    }
    // 启动软件定时器
    if (CloseLightOrM1_TASK != NULL) {
        xTimerStartFromISR(CloseLightOrM1_TASK, 0); // 开启关闭计时器
    }
    // 开启 M1
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableM1Card)) {
        M1_POWER_ON;
        I_SI522A_IO_Init();
        FL_DelayMs(500);
        PCD_SI522A_TypeA_Init();
    }
    // 开启 红外
    LPUART0_Init(false); // 修改 LPUart0 初始化
    Infrared_POWER_ON;
}
