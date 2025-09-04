#include "TaskAllReceive.h"
#include "LPUart_0_And_1_Lib.h"
#include "RTC_SetTime.h"
#include "AT24CXXDataLoader.h"
#include "PublicLib_No_One.h"
#include "StrLib.h"
#include "Display.h"
#include "event_groups.h"
#include "Task_Valve.h"
#include "CI522_App.h"
#include "RTC.h"
#include "TaskAboutTimer.h"
#include "PowerCtrl.h"
#include "TaskNBAndWeb.h"
#include "NetProt_Module.h"
#include "HY_TXRX.h"

TimerHandle_t CloseLightOrM1_TASK = NULL;
void OpenBSTime(bool IsOpenBSTime) {
    static bool getUartx = false;
    if (IsOpenBSTime == getUartx) {
        return; // 如果状态没有改变，则不执行任何操作
    }
    if (IsOpenBSTime == true) {
        BSTIM32_Setup();    //开启定时器中断
#ifdef OPEN_LOWPWER_DEBUG
        RTC_TASK.InitSetTimeTask(LowPwerDebug, 1, NULL);
#endif
    } else {
        BSTIM32_Stop();
    }
    getUartx = IsOpenBSTime;
}
//////////////////////////////////////////////////////////////
EventGroupHandle_t xEventGroup_Receive = NULL;
#define LPUart0_Rec      (1 << 0)
#define Uart1_Rec        (1 << 1)
#define SI522A_Rec       (1 << 2)
#define NBUart_Virtual   (1 << 3)
#define AllxEvent_Receive (LPUart0_Rec | Uart1_Rec | SI522A_Rec | NBUart_Virtual) 
//////////////////////////////////////////////////////////////
TaskHandle_t AllReceive_Hand = NULL;
SemaphoreHandle_t LPUartxSemaphore = NULL;
void LPUart0CheckBuff(void) {
    LPUart0DisableIRQ;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBits(xEventGroup_Receive, LPUart0_Rec);
    // 向 TimeTask 发出信号量, 准备继续运行
    xSemaphoreGiveFromISR(LPUartxSemaphore, &xHigherPriorityTaskWoken);
    // 恢复 Task 任务
    if (xTaskResumeFromISR(AllReceive_Hand) == pdFALSE) {
        // 如果需要切换上下文
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
// void Uart1CheckBuff(void) {
//     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//     xEventGroupSetBits(xEventGroup_Receive, Uart1_Rec);
//     // 向 TimeTask 发出信号量, 准备继续运行
//     xSemaphoreGiveFromISR(LPUartxSemaphore, &xHigherPriorityTaskWoken);
//     // 恢复 Task 任务
//     if (xTaskResumeFromISR(AllReceive_Hand) == pdFALSE) {
//         // 如果需要切换上下文
//         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     }
// }
///////////////////////////////////////
// Uart1 中间层
void NBUartCheckBuff(void) {
    Uart1DisableIRQ;
    // 在联网阶段，直接转存到 CmdTable
    if (!System_RunData.Now_NetDevParameter.NowNetOnlineFlag) {
        copyDataFormUART();
        Uart1EnableIRQ;
        return;
    }
    // 已在线，监控到特殊字符串
    if (System_RunData.Now_NetDevParameter.checkFlagStr != NULL) {
        if (strstr(UART_DATABUFF, System_RunData.Now_NetDevParameter.checkFlagStr) != NULL) {
            copyDataFormUART();
            CheckStr_OK();
            Uart1EnableIRQ;
            return;
        }
    }
    // 包含指令的字符串
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBits(xEventGroup_Receive, NBUart_Virtual);
    // 向 TimeTask 发出信号量, 准备继续运行
    xSemaphoreGiveFromISR(LPUartxSemaphore, &xHigherPriorityTaskWoken);
    // 恢复 Task 任务
    if (xTaskResumeFromISR(AllReceive_Hand) == pdFALSE) {
        // 如果需要切换上下文
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
///////////////////////////////////////
static uint8_t isReadCard = false;
void closeLightOrM1(void) {
    RTC_TASK.InitSetTimeTask(CloseReadM1, 1, closeLightOrM1);
    static uint8_t CallCount = 0;
    if (CallCount == 0) {
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableM1Card)) {
            isReadCard = true;
        }
    }
    CallCount++;
    if (CallCount == 30) {
        CallCount = 0;
        RTC_TASK.CloseTask(CloseReadM1);
        CloseLPUart0TTL();      // 关闭 红外
        M1_POWER_OFF;           // 关闭 M1
        isReadCard = false;
    }

    if ((isReadCard == true) && !(xEventGroupGetBits(xEventGroup_Receive) & SI522A_Rec)) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xEventGroupSetBits(xEventGroup_Receive, SI522A_Rec);
        // 向 TimeTask 发出信号量, 准备继续运行
        xSemaphoreGiveFromISR(LPUartxSemaphore, &xHigherPriorityTaskWoken);
        // 恢复 Task 任务
        if (xTaskResumeFromISR(AllReceive_Hand) == pdFALSE) {
            // 如果需要切换上下文
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
// 开关卡
void OpenOrCloseValveCard(void) {
    if (System_RunData.Now_Degree_Part < DegreePartToUint(30)) {
        SetValve TempSetdata = {
            .DoneId = VALVE_REASON_CARD_SWIPE,
            .MaxDelay_1000Tick = 5,
            .Set_Degree_Part = DegreePartToUint(0),
        };
        ValveCtrlStart(TempSetdata);
    } else {
        SetValve TempSetdata = {
            .DoneId = VALVE_REASON_CARD_SWIPE,
            .MaxDelay_1000Tick = 5,
            .Set_Degree_Part = DegreePartToUint(100),
        };
        ValveCtrlStart(TempSetdata);
    }
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
    setRtcDate(NowTimeData, true); // 设置时间
}
// 截止日期卡
void LimitCard(strnew SetLimitTime, uint16_t SetDegree) {
    if (!(0 < SetDegree && SetDegree < DegreePartToUint(100))) {
        Show_M1_Check_ErrerCode(0);
        return;
    }
    if (!Check_Date_ByHEX(0x20, SetLimitTime.Name._char[0], SetLimitTime.Name._char[1], SetLimitTime.Name._char[2])) {
        Show_M1_Check_ErrerCode(1);
        return;
    }
    newString(NowTimeStr, 30);
    sprintf(NowTimeStr.Name._char, "20%02x-%02x-%02x %02x:%02x:%02x", NowYear, NowMonth, NowDay, NowHour, NowMinute, NowSecond);
    uint32_t NowTime = getTimeNumber_UTCByRTCTime(NowTimeStr);
    memset(NowTimeStr.Name._char, 0, NowTimeStr.MaxLen);
    sprintf(NowTimeStr.Name._char, "20%02x-%02x-%02x 00:00:00",
        SetLimitTime.Name._char[0],
        SetLimitTime.Name._char[1],
        SetLimitTime.Name._char[2]
    );
    uint32_t MaxTime = getTimeNumber_UTCByRTCTime(NowTimeStr);
    SetValve TempSetdata = {
        .DoneId = VALVE_REASON_SYSTEM_DEADLINE,
        .MaxDelay_1000Tick = 5,
        .Set_Degree_Part = 0xFFFF,
    };
    if (NowTime > MaxTime) {
        TempSetdata.Set_Degree_Part = 0;
    } else {
        if (System_RunData.Now_Degree_Part < DegreePartToUint(30)) {
            TempSetdata.Set_Degree_Part = SetDegree;
        }
    }
    if (TempSetdata.Set_Degree_Part != 0xFFFF) {
        ValveCtrlStart(TempSetdata);
    }
    memcpy(AT24CXX_Manager_NET.LimitTime, NowTimeStr.Name._char, ARR_SIZE(AT24CXX_Manager_NET.LimitTime));
    isRunEEprom();
    EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.LimitTime, sizeof(AT24CXX_Manager_NET.LimitTime));
    clearEEpromFlag;
}
// 执行刷卡命令
bool DoneCardCmd(UserCard * _UserCardData) {
    UserCard UserCardData = {0};
    UserCardData = (*_UserCardData);
    if (!(UserCardData.MeterID[0] == 0xFF) && (UserCardData.MeterID[1] == 0xFF) && (UserCardData.MeterID[2] == 0xFF) && (UserCardData.MeterID[3] == 0xFF)) {
        if (!(AT24CXX_Manager_NET.MeterID[0] == UserCardData.MeterID[0]) &&
            (AT24CXX_Manager_NET.MeterID[1] == UserCardData.MeterID[1]) &&
            (AT24CXX_Manager_NET.MeterID[2] == UserCardData.MeterID[2]) &&
            (AT24CXX_Manager_NET.MeterID[3] == UserCardData.MeterID[3])) {
            Show_M1_Check_ErrerCode(2);
            return false;
        }
    }
    switch (UserCardData.CardType) {
        case 0:
            OpenOrCloseValveCard();
            break;
        case 1:
            SetRTCtimeCard(NEW_NAME(UserCardData.RTC_Time_Set));
            break;
        case 2:
            LimitCard(NEW_NAME(UserCardData.LimitTime), UserCardData.LimitTimeDegree);
            break;
        case 3:
            StartValveAutoTest();   // 自检卡
            break;
        case 4:
            // 权限卡
            AT24CXX_Manager_NET.ModeCode = UserCardData.ModeCode;
            break;
        default:
            return false;
    }
    return true;
}

#include "NetProt_Module.h"
void AllReceive(void * pvParameters) {
    LPUartxSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    // 创建事件组
    xEventGroup_Receive = xEventGroupCreate();
    uint8_t UartX = 0xFF;
    uint16_t * UartXLen = NULL;
    char * UartBuff = NULL;
    while (xSemaphoreTake(LPUartxSemaphore, portMAX_DELAY) == pdTRUE) {
        if (xEventGroupGetBits(xEventGroup_Receive) & NBUart_Virtual) {
            xEventGroupClearBits(xEventGroup_Receive, NBUart_Virtual);   // 清除事件标志位
            getHYCmdFormATStr(NEW_NAME(UART1Ddata.RxBuf), UART1Ddata.RxLen);
            xEventGroupSetBits(xEventGroup_Receive, Uart1_Rec);
            UartX = 0xF2;
        }
        if (xEventGroupGetBits(xEventGroup_Receive) & LPUart0_Rec) {
            xEventGroupClearBits(xEventGroup_Receive, LPUart0_Rec);   // 清除事件标志位
            UartX = 0xF1;
            UartBuff = LPUART0Ddata.RxBuf;
            UartXLen = &LPUART0Ddata.RxLen;
            memset(LPUART0Ddata.TxBuf, 0, LONG_UARTMAX);
        }
        if (xEventGroupGetBits(xEventGroup_Receive) & Uart1_Rec) {
            xEventGroupClearBits(xEventGroup_Receive, Uart1_Rec);   // 清除事件标志位.
            UartX = 0xF2;
            UartBuff = CmdTable.DataBuff_RX;
            UartXLen = &CmdTable.NowRX_Len;
            memset(CmdTable.DataBuff_TX, 0, CmdStrLenMax);
        }
        if (UartX == 0xF1 || UartX == 0xF2) {
            System_RunData.CtrlDev_Set_Degree_Part = 0xFFFF;
            SetValve TempSetdata = {
                .DoneId = VALVE_REASON_INFRARED_INTERFACE,
                .MaxDelay_1000Tick = 1,
                .Set_Degree_Part = System_RunData.CtrlDev_Set_Degree_Part,
            };
            uint8_t RecCmdFlag = HY_USB_TTL_CheckBuff(UartBuff, (*UartXLen), UartX);
            if (RecCmdFlag == false) {
                LPUart0EnableIRQ;
                Uart1EnableIRQ;
                Clr_LPUart0_RxBuffer();
                Clr_Uart1_RxBuffer();
                SetTime.CloseTask(RecTimeLPUART0);
                OpenBSTime(false); // 关闭定时器
                UartX = 0xFF;
                continue;
            }
            LPUart0EnableIRQ;
            Uart1EnableIRQ;
            if ((*UartXLen) != 0) {
                if (UartX == 0xF1) {    // 红外
                    LPUART0_Send((unsigned char *)LPUART0Ddata.TxBuf, LPUART0Ddata.TxLen);
                } else {
                    PackHYDataToAtData(CmdTable.DataBuff_TX, CmdTable.NowTX_Len, NEW_NAME(UART1Ddata.TxBuf));
                    sendDataByTTLProt(UART1Ddata.TxBuf, UART1Ddata.TxLen);
                    System_RunData.ReceiveFlag = true;
                }
            }
            Clr_LPUart0_RxBuffer();
            Clr_Uart1_RxBuffer();
            SetTime.CloseTask(RecTimeLPUART0);
            OpenBSTime(false); // 关闭定时器
            if ((_Module_Start_Flage == 0) && (RecCmdFlag == 0xFF)) { // 非网络设置
                EEprom_AT24CXX_Parameter_Init(true);
                LCD_MenuPage_Change(0);
                setIsWriteEEprom(false);
            }
            if (System_RunData.CtrlDev_Set_Degree_Part == 0xEEEE) {
                StartValveAutoTest();
                System_RunData.CtrlDev_Set_Degree_Part = 0xFFFF;
            }
            if (System_RunData.CtrlDev_Set_Degree_Part != 0xFFFF) {
                if (UartX == 0xF1) {
                    TempSetdata.DoneId = VALVE_REASON_INFRARED_INTERFACE;       // H 红外接口
                } else {
                    TempSetdata.DoneId = VALVE_REASON_NB_MQTT_ONENET_RESERVE;   // G NB MQTT OneNet
                }
                TempSetdata.Set_Degree_Part = System_RunData.CtrlDev_Set_Degree_Part;
                ValveCtrlStart(TempSetdata);
            }
            UartX = 0xFF;
        }
        if (xEventGroupGetBits(xEventGroup_Receive) & SI522A_Rec) {
            OpenBSTime(true);
            if (PCD_CI522_TypeA_rw_block(DoneCardCmd)) {
                isReadCard = false;
            }
            xEventGroupClearBits(xEventGroup_Receive, SI522A_Rec);   // 清除事件标志位
            OpenBSTime(false); // 关闭定时器
        }
    }
}
/////////////////////////////////////////////
void OpenUartOfLightOrM1(void) {
    // 启动软件定时器
    if (RTC_TASK.Task[CloseReadM1].isTaskStart == true) {
        return; // 定时器已启动，无需重复启动
    }
    RTC_TASK.InitSetTimeTask(CloseReadM1, 1, closeLightOrM1);
    // 开启 M1
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableM1Card)) {
        M1_POWER_ON;
        IncludeDelayMs(50);
        PCD_CI522_TypeA_Init();
    }
    // 开启 红外
    LPUART0_Init(); // 修改 LPUart0 初始化
    MF_LPUART0_Interrupt_Init();
    Infrared_POWER_ON;
}
