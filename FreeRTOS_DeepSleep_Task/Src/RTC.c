#include "fm33lc0xx_fl.h"
#include <string.h>
#include <stdio.h>
#include "RTC.h"
#include "Define.h"
#include "main.h"
#include "AT24C02.h"
#include "PublicLib_No_One.h"
#include "StrLib.h"
#include "RTC_SetTime.h"
#include "NumberBaseLib.h"
#include "AT24CXXDataLoader.h"
#include "LPUart_0_And_1_Lib.h"
#include "NB_Mode_Lib.h"

// 内部 RTC ***************************
FL_RTC_InitTypeDef * _RTC_Date;
// 获取每月的天数
uint32_t getDaysInMonth(uint32_t year, uint32_t month) {
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    }
    uint32_t daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return daysInMonth[(month - 1) % 12];
}
// RTC 时间增加秒数
FL_RTC_InitTypeDef RTCTimeAddSecToNewTime(FL_RTC_InitTypeDef NowTime, uint32_t SecNum, uint8_t isDex) {
    if (isDex) {
        NowTime.year = anyBaseToAnyBase((uint64_t)NowTime.year, 10, 16); // 转换
        NowTime.month = anyBaseToAnyBase((uint64_t)NowTime.month, 10, 16); // 转换
        NowTime.day = anyBaseToAnyBase((uint64_t)NowTime.day, 10, 16); // 转换
        NowTime.hour = anyBaseToAnyBase((uint64_t)NowTime.hour, 10, 16); // 转换
        NowTime.minute = anyBaseToAnyBase((uint64_t)NowTime.minute, 10, 16); // 转换
        NowTime.second = anyBaseToAnyBase((uint64_t)NowTime.second, 10, 16); // 转换
    }

    NowTime.second += SecNum;    // 添加秒数
    // 处理溢出
    while (NowTime.second >= 60) {
        NowTime.second -= 60;
        NowTime.minute++;
    }
    while (NowTime.minute >= 60) {
        NowTime.minute -= 60;
        NowTime.hour++;
    }
    while (NowTime.hour >= 24) {
        NowTime.hour -= 24;
        NowTime.day++;
    }

    // 处理天数溢出
    while (NowTime.day > getDaysInMonth(NowTime.year, NowTime.month)) {
        NowTime.day -= getDaysInMonth(NowTime.year, NowTime.month);
        NowTime.month++;
        if (NowTime.month > 12) {
            NowTime.month = 1;
            NowTime.year++;
        }
    }

    if (isDex) {
        NowTime.year = anyBaseToAnyBase((uint64_t)NowTime.year, 16, 10); // 转换
        NowTime.month = anyBaseToAnyBase((uint64_t)NowTime.month, 16, 10); // 转换
        NowTime.day = anyBaseToAnyBase((uint64_t)NowTime.day, 16, 10); // 转换
        NowTime.hour = anyBaseToAnyBase((uint64_t)NowTime.hour, 16, 10); // 转换
        NowTime.minute = anyBaseToAnyBase((uint64_t)NowTime.minute, 16, 10); // 转换
        NowTime.second = anyBaseToAnyBase((uint64_t)NowTime.second, 16, 10); // 转换
    }
    NowWeek = getDayOfWeek(NowYear, NowMonth, NowDay); // 计算周 周不需要转换
    return NowTime;
}

// 计算周
int getDayOfWeek(uint32_t iYear, uint32_t iMonth, uint32_t iDay) {
    int iWeek = 0;
    unsigned int y = 0, c = 0, m = 0, d = 0;

    iYear = anyBaseToAnyBase(iYear, 10, 16);
    iMonth = anyBaseToAnyBase(iMonth, 10, 16);
    iDay = anyBaseToAnyBase(iDay, 10, 16);

    if (iMonth == 1 || iMonth == 2) {
        c = (iYear - 1) / 100;
        y = (iYear - 1) % 100;
        m = iMonth + 12;
        d = iDay;
    } else {
        c = iYear / 100;
        y = iYear % 100;
        m = iMonth;
        d = iDay;
    }

    iWeek = y + y / 4 + c / 4 - 2 * c + 26 * (m + 1) / 10 + d - 1; //蔡勒公式
    iWeek = iWeek >= 0 ? (iWeek % 7) : (iWeek % 7 + 7); //iWeek为负时取模
    if (iWeek == 0) //星期日不作为一周的第一天
    {
        iWeek = 7;
    }

    return iWeek;
}

unsigned char Check_Date_ByHEX(unsigned char H_year1, unsigned char H_year2, unsigned char H_month, unsigned char H_date) {
    unsigned int w_year;
    unsigned char w_month, w_date;
    char Month_buf[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    w_year = (H_year1 / 0x10) * 1000 + (H_year1 % 0x10) * 100 + (H_year2 / 0x10) * 10 + (H_year2 % 0x10);
    w_month = (H_month / 0x10) * 10 + (H_month % 0x10);
    w_date = (H_date / 0x10) * 10 + (H_date % 0x10);
    if (w_month == 2)(((w_year % 4 == 0) && (w_year % 100 != 0)) || (w_year % 400 == 0)) ? Month_buf[1] += 1 : Month_buf[1];
    if (w_month > 12) return 0;
    if (w_date > 31) return 0;
    else if (w_month<1 || w_date>Month_buf[w_month - 1] || w_date < 1) return 0;
    return 1;
}

unsigned char Check_Time_ByHEX(unsigned char H_hour, unsigned char H_min, unsigned char H_second) {
    unsigned char w_hour, w_min, w_second;
    w_hour = (H_hour >> 4) * 10 + (H_hour & 0x0F);
    w_min = (H_min >> 4) * 10 + (H_min & 0x0F);
    w_second = (H_second >> 4) * 10 + (H_second & 0x0F);
    if ((w_hour == 0) || ((w_hour > 0) && (w_hour < 24))) {
        if ((w_min == 0) || ((w_min > 0) && (w_min < 60))) {
            if ((w_second == 0) || ((w_second > 0) && (w_second < 60))) {
                return 1;
            } else return 0;
        } else return 0;
    } else return 0;
}

void RTC_IRQ_Second(void) {
    FL_NVIC_ConfigTypeDef    InterruptConfigStruct;
    FL_RCC_EnableGroup1BusClock(FL_RCC_GROUP1_BUSCLK_RTC);
    FL_RTC_EnableIT_Second(RTC);
    InterruptConfigStruct.preemptPriority = 0x000C;
    FL_NVIC_Init(&InterruptConfigStruct, RTC_IRQn);
}
void MF_RTC_1S_Init(void) {
    _RTC_Date = &AT24CXX_Manager.Time_Data;
    FL_RTC_Init(RTC, &RTC_Date);

    IncludeDelayMs(1000);
    RTC_IRQ_Second();
}
/************************************************************************************/
static bool IsWriteRTCFlag = false;
static void RTC_GetRTC(FL_RTC_InitTypeDef * InitStructer) {
    unsigned char i;
    FL_RTC_InitTypeDef TempTime1, TempTime2;
    for (uint8_t n = 0; n < 3; n++) {
        FL_RTC_GetTime(RTC, &TempTime1);
        FL_RTC_GetTime(RTC, &TempTime2);
        for (i = 0; i < 7; i++) {
            if (((unsigned long int *)(&TempTime1))[i] != ((unsigned long int *)(&TempTime2))[i]) {
                break;
            }
        }
        if (i == 7) {
            memcpy((unsigned long int *)(InitStructer), (unsigned long int *)(&TempTime1), 7 * sizeof(unsigned long int));
            break;
        }
    }
    return;
}
void RTC_SetRTC(FL_RTC_InitTypeDef * InitStructer) {
    if (!checkTimeFrom(*InitStructer)) {
        return;
    }

    IsWriteRTCFlag = true;
    for (int i = 0; (i < 1000 && IsWriteRTCFlag); i++) {
        IncludeDelayMs(1);
    }
    (*InitStructer).week = getDayOfWeek((*InitStructer).year, (*InitStructer).month, (*InitStructer).day);
    FL_RTC_ConfigTime(RTC, InitStructer);
    RTC_GetRTC(&RTC_Date);  // 获取时间
    return;
}
// 设置时间
void setRtcDate(strnew SteStrRTCData, char isWriteEEPROM) {
    FL_RTC_InitTypeDef SetRTC = {0};
    char * NowAddr = SteStrRTCData.Name._char; // 初始化 NowAddr "2024-07-05 15:44:03"
    sscanf(NowAddr, "%d-%d-%d %d:%d:%d", &SetRTC.year, &SetRTC.month, &SetRTC.day, &SetRTC.hour, &SetRTC.minute, &SetRTC.second);

    SetRTC.year = (uint32_t)anyBaseToAnyBase((uint64_t)SetRTC.year, 16, 10); // 将 SetRTC.year 16进制转换成10进制
    SetRTC.month = (uint32_t)anyBaseToAnyBase((uint64_t)SetRTC.month, 16, 10); // 将 SetRTC.month 16进制转换成10进制
    SetRTC.day = (uint32_t)anyBaseToAnyBase((uint64_t)SetRTC.day, 16, 10); // 将 SetRTC.day 16进制转换成10进制
    SetRTC.hour = (uint32_t)anyBaseToAnyBase((uint64_t)SetRTC.hour, 16, 10); // 将 SetRTC.hour 16进制转换成10进制
    SetRTC.minute = (uint32_t)anyBaseToAnyBase((uint64_t)SetRTC.minute, 16, 10); // 将 SetRTC.minute 16进制转换成10进制
    SetRTC.second = (uint32_t)anyBaseToAnyBase((uint64_t)SetRTC.second, 16, 10); // 将 SetRTC.second 16进制转换成10进制
    SetRTC.week = getDayOfWeek(SetRTC.year, SetRTC.month, SetRTC.day); // 计算周 周不需要转换, 因为 1-7 的 16进制 和 10进制 相同

    RTC_SetRTC(&SetRTC);    // 设置时间结构体

    if (isWriteEEPROM) {
        // 写 eeprom
        EEprom_AT24C0XXData_Write(&AT24CXX_Manager.Time_Data, sizeof(FL_RTC_InitTypeDef));
    }
}
void getStrUserTime(FL_RTC_InitTypeDef UserDate, char NowStrRTCData[]) {
    // 年月日时分秒
    sprintf(NowStrRTCData, "20%02x-%02x-%02x %02x:%02x:%02x", UserDate.year, UserDate.month, UserDate.day, UserDate.hour, UserDate.minute, UserDate.second);
}
/************************************************************************************/

void _RTC_ReBoot_Check(struct Wifi_Reboot This);
void _EC20_ReBoot_In_RTC_By_1Second(struct Wifi_Reboot This, char sec);

Wifi_Reboot Reboot = {
    .ReBoot_Falge = 0,
    .ReBoot_1Second_Delay = 2,
    .RTC_ReBoot_Check = _RTC_ReBoot_Check,
    .EC20_ReBoot_In_RTC_By_1Second = _EC20_ReBoot_In_RTC_By_1Second,
};

void _EC20_ReBoot_In_RTC_By_1Second(struct Wifi_Reboot This, char sec) {
    This.ReBoot_Falge = 1;
    This.ReBoot_1Second_Delay = sec;
}

void _RTC_ReBoot_Check(struct Wifi_Reboot This) {
    if (This.ReBoot_Falge == 1) {
        This.ReBoot_1Second_Delay--;
        if (This.ReBoot_1Second_Delay < 0)
            NVIC_SystemReset();
    }
}

void RTC_IRQHandler(void) {
    if (FL_ENABLE == FL_RTC_IsEnabledIT_Second(RTC) && FL_SET == FL_RTC_IsActiveFlag_Second(RTC)) {
        FL_RTC_ClearFlag_Second(RTC);   // 清除中断标志
        RTC_GetRTC(&RTC_Date);          // 获取时间
        SecondCountRTCTask();
        Reboot.RTC_ReBoot_Check(&Reboot);   // 异步重启
        if (NowSecond == 0x00) {
#ifdef _MINTimeTaskMAX
            MinCountRTCTask();
#endif
            if (NowMinute == 0x00) {
                // SaveDevData();
                NowWeek = getDayOfWeek(NowYear, NowMonth, NowDay); // 计算周 周不需要转换
                if (NowHour == 0x00) {
                }
                if (NowHour == AT24CXX_Manager_NET.SendManageObj.SendStartHour) {
                    InitSendModeAndTimeTask();  // 起始点刷新任务 计算当天的发送模式和时间任务
                }
            }
            if ((NowMinute % 0x10) == 0) {
#ifdef Min_10_TimeTaskMAX
                Min_10_CountRTCTask();
#endif
            }
            if ((NowMinute % 0x15) == 0) {
#ifdef Min_15_TimeTaskMAX
                Min_15_CountRTCTask();
#endif
            }
        }
        if (IsWriteRTCFlag == true) {
            IsWriteRTCFlag = false; // 确保中断刚结束, 就写入RTC
        }
    }
}

