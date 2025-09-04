#include "RTC.h"
#include "NB_Mode_Lib.h"
#include "AT24CXXDataLoader.h"
#include "LPUart_0_And_1_Lib.h"

void NB_Mode_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct;
    // PC5_NB_PWR_EN 
    GPIO_InitStruct.pin = FL_GPIO_PIN_5;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    NB_PWR_LOW;
    MF_UART1_Init();
    MF_UART1_Interrupt_Init();
    CloseUart1TTL();
    return;
}

// 获取当前日距离1月1日的天数
int daysFormOneMonthOneDay(int year, int month, int day) {
    int days = 0;
    for (int i = 1; i < month; i++) {
        if (i == 2) {
            days += isLeapYear(year) ? 29 : 28;
        } else if (i == 4 || i == 6 || i == 9 || i == 11) {
            days += 30;
        } else {
            days += 31;
        }
    }
    days += day;
    return days;
}

// 根据输入的当前天数, 判断应该处于什么模式
char readNowSendMode(int DayNumFormStartDay) {
    uint16_t NowSendFlagMode = 0;
    uint16_t StartDay[4] = {
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[0].StartDay, // {105, 7, 4}
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[1].StartDay, // {258, 1, 4}
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[2].StartDay, // {288, 1, 4}
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[3].StartDay, // {305, 1, 4}
    };

    //对 StartDay 冒泡排序
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3 - i - 1; j++) {
            if (StartDay[j] > StartDay[j + 1]) {
                uint16_t temp = StartDay[j];
                StartDay[j] = StartDay[j + 1];
                StartDay[j + 1] = temp;
            }
        }
    }
    if ((StartDay[0] >= StartDay[1]) || (StartDay[1]) >= (StartDay[2])) {
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[0].StartDay = 105;
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[0].SendInterval = 7;
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[0].OneDayMaxSendTime = 4;

        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[1].StartDay = 258;
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[1].SendInterval = 1;
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[1].OneDayMaxSendTime = 4;

        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[2].StartDay = 288;
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[2].SendInterval = 1;
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[2].OneDayMaxSendTime = 4;

        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[3].StartDay = 305;
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[3].SendInterval = 1;
        AT24CXX_Manager_NET.SendManageObj.SendFlagMode[3].OneDayMaxSendTime = 4;
        EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.SendManageObj.SendFlagMode, sizeof(AT24CXX_Manager_NET.SendManageObj.SendFlagMode));
        return readNowSendMode(DayNumFormStartDay);
    } else {
        if ((StartDay[0] < DayNumFormStartDay) && (DayNumFormStartDay < StartDay[1])) {
            NowSendFlagMode = StartDay[0];  //暂时记录对应模式的参数值
        } else if ((StartDay[1] < DayNumFormStartDay) && (DayNumFormStartDay < StartDay[2])) {
            NowSendFlagMode = StartDay[1];  //暂时记录对应模式的参数值
        } else if ((StartDay[2] < DayNumFormStartDay) && (DayNumFormStartDay < StartDay[3])) {
            NowSendFlagMode = StartDay[2];  //暂时记录对应模式的参数值
        } else {
            NowSendFlagMode = StartDay[3];  //暂时记录对应模式的参数值
        }
        // 循环对比 NowSendFlagMode 与 AT24CXX_Manager_NET.SendManageObj.SendFlagMode[x].StartDay
        for (int i = 0; i < 4; i++) {
            if (NowSendFlagMode == AT24CXX_Manager_NET.SendManageObj.SendFlagMode[i].StartDay) {
                NowSendFlagMode = i;
            }
        }
        return NowSendFlagMode;
    }
}
// 每天 0 点初始化发送模式及定时器任务
uint32_t InitSendModeAndTimeTask(void) {
    // ************************************ 判断当天属于什么模式 ************************************ //
    uint32_t Now_TimeToSec = anyBaseToAnyBase(NowHour, 10, 16) * 3600 + anyBaseToAnyBase(NowMinute, 10, 16) * 60 + anyBaseToAnyBase(NowSecond, 10, 16);
    // 获取当前日距离1月1日的天数
    uint16_t DayNumFormStartDay = daysFormOneMonthOneDay(GetDateHex(NowYear), GetDateHex(NowMonth), GetDateHex(NowDay));
    char NowSendFlagMode = readNowSendMode(DayNumFormStartDay);

    // ************************************ 根据发送模式设置任务 ************************************ // 
    if (AT24CXX_Manager_NET.SendManageObj.SendStartHour > 23) {
        AT24CXX_Manager_NET.SendManageObj.SendStartHour = 6;
        EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.SendManageObj.SendStartHour, sizeof(AT24CXX_Manager_NET.SendManageObj.SendStartHour));
        IncludeDelayMs(200);
    }
    if (AT24CXX_Manager_NET.SendManageObj.SendIntervalDay == 0) {
        AT24CXX_Manager_NET.SendManageObj.SendIntervalDay = 6 * 3600;
        EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.SendManageObj.SendStartHour, sizeof(AT24CXX_Manager_NET.SendManageObj.SendStartHour));
        IncludeDelayMs(200);
    }
    // 计算离散值
    newString(IDToNumStr, 4);
    strnew IDEnd2Char = {0};
    IDEnd2Char.Name._char = &AT24CXX_Manager_NET.MeterID[2];
    IDEnd2Char.MaxLen = 2;
    shortChStrToDoubleChStr(IDEnd2Char, IDToNumStr);
    swapChr(&IDToNumStr.Name._char[0], &IDToNumStr.Name._char[2]);
    uint32_t TempMeterIdToSec = anyBaseArrayToAnyBaseNumber(IDToNumStr.Name._char, IDToNumStr.MaxLen, 10, 16);
    TempMeterIdToSec = (((TempMeterIdToSec * 3000) / 9999) % 3001); // 离散值对 3600s 取余

    uint32_t TempSendStartHourToSec = (AT24CXX_Manager_NET.SendManageObj.SendStartHour * 3600) + TempMeterIdToSec; // 转换为秒
    if (Now_TimeToSec >= TempSendStartHourToSec) {
        if (Now_TimeToSec == TempSendStartHourToSec) {
            _Module_Start_Flage = _Module_AutoSend;
        }
        TempSendStartHourToSec = AT24CXX_Manager_NET.SendManageObj.SendIntervalDay - \
            (Now_TimeToSec - TempSendStartHourToSec) % (AT24CXX_Manager_NET.SendManageObj.SendIntervalDay);
    } else {
        TempSendStartHourToSec = (TempSendStartHourToSec - Now_TimeToSec) % (AT24CXX_Manager_NET.SendManageObj.SendIntervalDay);
    }

    int32_t DayNum = DayNumFormStartDay - AT24CXX_Manager_NET.SendManageObj.SendFlagMode[NowSendFlagMode].StartDay;
    DayNum = (DayNum > 0 ? DayNum : 0 - DayNum);
    DayNum = (DayNum % AT24CXX_Manager_NET.SendManageObj.SendFlagMode[NowSendFlagMode].SendInterval);
    if (!DayNum) {
        if (NowHour > AT24CXX_Manager_NET.SendManageObj.SendStartHour - (AT24CXX_Manager_NET.SendManageObj.SendIntervalDay / 3600)) {
            RTC_TASK.InitSetTimeTask(SendIntervalTask, TempSendStartHourToSec, NULL); // start + 离散点
        } else {
            RTC_TASK.CloseTask(SendIntervalTask); // 当天的任务强制关闭 
            TempSendStartHourToSec += AT24CXX_Manager_NET.SendManageObj.SendIntervalDay; // 计算下一个发送时间
        }
    } else {
        DayNum = AT24CXX_Manager_NET.SendManageObj.SendFlagMode[NowSendFlagMode].SendInterval - DayNum;
        if (Now_TimeToSec >= ((AT24CXX_Manager_NET.SendManageObj.SendStartHour * 3600) + TempMeterIdToSec - AT24CXX_Manager_NET.SendManageObj.SendIntervalDay)) {
            // 当前时间小于起始点
            RTC_TASK.CloseTask(SendIntervalTask); // 当天的任务强制关闭 
            TempSendStartHourToSec = (DayNum * 86400) + TempSendStartHourToSec;
        } else {
            if (DayNum + 1 == AT24CXX_Manager_NET.SendManageObj.SendFlagMode[NowSendFlagMode].SendInterval) { // 如果昨天刚传 
                RTC_TASK.InitSetTimeTask(SendIntervalTask, TempSendStartHourToSec, NULL); // 没到计算点，需要启动定时器
            }
        }
    }
    return TempSendStartHourToSec;
}

