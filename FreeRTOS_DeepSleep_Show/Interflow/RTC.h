#ifndef RTC_H
#define RTC_H

#include "../Interflow/Define.h"
#include <stdint.h>
#include "../Interflow/StrLib.h"
#include "../Interflow/RTC_SetTime.h"

/*-----------------------------------定义类----------------------------------*/
typedef struct Wifi_Reboot {
    unsigned char ReBoot_Falge;
    int ReBoot_1Second_Delay;
    void (*EC20_ReBoot_In_RTC_By_1Second)(struct Wifi_Reboot This, char sec); // 重启接口（sec 秒后重启 WIFI）
    void (*RTC_ReBoot_Check)(struct Wifi_Reboot This);              // 将数据整理成json字符串
} Wifi_Reboot;
extern Wifi_Reboot Reboot;
extern FL_RTC_InitTypeDef * _RTC_Date;
#define RTC_Date (*_RTC_Date)

// extern unsigned char RTC_Heart_LED_Count;
// extern void Set_Heart_LED_ON();

extern int isLeapYear(uint32_t year);
extern FL_RTC_InitTypeDef RTCTimeAddSecToNewTime(FL_RTC_InitTypeDef NowTime, uint32_t SecNum, uint8_t isDex);
#define RTCTime_Hex_AddSecToNewTime(TempTime, SecNum) RTCTimeAddSecToNewTime(TempTime, SecNum, 0)
#define RTCTime_Dex_AddSecToNewTime(TempTime, SecNum) RTCTimeAddSecToNewTime(TempTime, SecNum, 1)
extern long get_timestamp(void);
extern int getDayOfWeek(uint32_t iYear, uint32_t iMonth, uint32_t iDay);
extern void setRtcDate(strnew SteStrRTCData, char isWriteEEPROM);
extern void getStrUserTime(FL_RTC_InitTypeDef UserDate, char NowStrRTCData[]);
extern void getStrNowDataAndTimeByRTC(char NowStrRTCData[]);

// 外部
extern unsigned char Check_Time_ByHEX(unsigned char H_hour, unsigned char H_min, unsigned char H_second);
extern unsigned char Check_Date_ByHEX(unsigned char H_year1, unsigned char H_year2, unsigned char H_month, unsigned char H_date);
extern void RTC_SetRTC(FL_RTC_InitTypeDef * InitStructer);
extern void MF_RTC_1S_Init(void);

#endif
