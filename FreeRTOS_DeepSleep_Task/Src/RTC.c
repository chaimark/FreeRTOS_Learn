#include "fm33lc0xx_fl.h"
#include <string.h>
#include <stdio.h>
#include "RTC.h"
#include "Define.h"
#include "Main.h"
#include "AT24C02.h"
#include "PublicLib_No_One.h"
#include "StrLib.h"
#include "RTC_SetTime.h"
#include "NumberBaseLib.h"
#include "AT24CXXDataLoader.h"

// 内部 RTC ***************************
FL_RTC_InitTypeDef * _RTC_Date;
// 判断是否为闰年
int isLeapYear(uint32_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

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
    // 添加秒数
    NowTime.second += SecNum;
    // 处理溢出
    if (isDex) {
        while (NowTime.second >= 0x60) {
            NowTime.second -= 0x60;
            NowTime.minute++;
        }
        while (NowTime.minute >= 0x60) {
            NowTime.minute -= 0x60;
            NowTime.hour++;
        }
        while (NowTime.hour >= 0x24) {
            NowTime.hour -= 0x24;
            NowTime.day++;
        }
    } else {
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
    return NowTime;
}

// 计算从1970年1月1日到指定日期的总天数
int calculate_days(int year, int month, int day) {
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int total_days = 0;
    int i;

    // 计算从1970年到当前年份的总天数
    for (i = 1970; i < year; i++) {
        total_days += isLeapYear(i) ? 366 : 365;
    }

    // 计算当前年份从1月1日到指定日期的天数
    for (i = 0; i < month - 1; i++) {
        total_days += days_in_month[i];
        if (i == 1 && isLeapYear(year)) { // 2月需要额外加一天（闰年）
            total_days++;
        }
    }
    // 加上当前月份的天数
    total_days += day - 1;
    return total_days;
}

// 计算时间戳（秒数）
long get_timestamp(void) {
    // 计算从1970年1月1日到指定日期的总天数
    int days = calculate_days(NowYear, NowMonth, NowDay);
    // 转换为秒数
    long OverTimeSec = days * 86400LL;  // 每天86400秒
    OverTimeSec += NowHour * 3600LL;            // 小时转秒
    OverTimeSec += NowMinute * 60LL;            // 分钟转秒
    OverTimeSec += NowSecond;                   // 加上秒
    return OverTimeSec;
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
    InterruptConfigStruct.preemptPriority = 0x0001;
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

    FL_RTC_ConfigTime(RTC, InitStructer);
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
    RTC_GetRTC(&RTC_Date);  // 获取时间

    if (isWriteEEPROM) {
        // 写 eeprom
        EEprom_AT24C0XXData_Write(&AT24CXX_Manager.Time_Data, sizeof(FL_RTC_InitTypeDef));
    }
}
void getStrUserTime(FL_RTC_InitTypeDef UserDate, char NowStrRTCData[]) {
    // 年月日时分秒
    sprintf(NowStrRTCData, "20%02x-%02x-%02x %02x:%02x:%02x", UserDate.year, UserDate.month, UserDate.day, UserDate.hour, UserDate.minute, UserDate.second);
}
void getStrNowDataAndTimeByRTC(char NowStrRTCData[]) {
    FL_RTC_GetTime(RTC, &RTC_Date); // 获取时间
    getStrUserTime(RTC_Date, NowStrRTCData);
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
        CountRTCTask();
        Reboot.RTC_ReBoot_Check(&Reboot);   // 异步重启
        if (NowSecond == 0x00) {
            if (NowMinute == 0x00) {

                if (NowHour == 0x00) {
                    NowWeek = getDayOfWeek(NowYear, NowMonth, NowDay); // 计算周 周不需要转换
                }
            }
            if ((NowMinute % 0x10) == 0) {
            }
            if ((NowMinute % 0x15) == 0) {
            }
        }
        if (IsWriteRTCFlag == true) {
            IsWriteRTCFlag = false; // 确保中断刚结束, 就写入RTC
        }
    }
}

