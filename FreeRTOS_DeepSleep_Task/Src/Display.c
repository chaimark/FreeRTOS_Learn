#include <string.h>
#include "fm33lc0xx_fl.h"
#include "Display.h"
#include "Main.h"
#include "RTC.h"
#include "../Interflow/PublicLib_No_One.h"
#include "../Interflow/NumberBaseLib.h"
#include "../Interflow/StrLib.h"
#include "../Interflow/AT24CXXDataLoader.h"
#include "MotorCtrlDev.h"
#include "Task_Valve.h"
#include "StrLib.h"
#include "UpData.h"

/*********************Menu***************************************************************/
#define MaxMenuNum 20
typedef void (*Display)(void);
static uint8_t MENU_ID = 0;

void ShowValvePart(void) {
    uint16_t InputData = 0;
    if (System_RunData.Now_Degree_Part > DegreePartToUint(95)) {
        InputData = DegreePartToUint(100);
    } else if (System_RunData.Now_Degree_Part < DegreePartToUint(4)) {
        InputData = 0;
    } else {
        InputData = System_RunData.Now_Degree_Part;
    }
    if (InputData <= DegreePartToUint(5)) {
        LCD_ChineseClose_VALVE;
    } else if (InputData >= DegreePartToUint(95)) {
        LCD_ChineseOpen_VALVE;
    }
    unsigned char CountBuf[5] = {0};
    InputData = anyBaseNumberToAnyBaseArray(InputData, 16, 10, (char *)CountBuf, 5);
    LCD_SetDotPx(1, true);
    if ((int16_t)InputData <= 0) {
        LCD_Num_Data(0, 2);
        LCD_Num_Data(0, 1);
        return;
    }
    for (uint8_t i = 0; i < InputData; i++) {
        LCD_Num_Data(CountBuf[i], (InputData - i));
    }
    return;
}

void ShowTemper(void) {
    unsigned char CountBuf[4] = {0};
    int InputData = floatToint6410Rate(System_RunData.Now_Temper_T1);
    InputData = anyBaseNumberToAnyBaseArray(InputData, 16, 10, (char *)CountBuf, 4);
    LCD_SetDotPx(5, true);
    if (InputData <= 0) {
        LCD_Num_Data(0, 6);
        LCD_Num_Data(0, 5);
    } else {
        for (uint8_t i = 0; i < InputData; i++) {
            LCD_Num_Data(CountBuf[i], (InputData - i + 4));
        }
    }
    InputData = floatToint6410Rate(System_RunData.Now_Temper_T2);
    InputData = anyBaseNumberToAnyBaseArray(InputData, 16, 10, (char *)CountBuf, 4);
    LCD_SetDotPx(1, true);
    if (InputData <= 0) {
        LCD_Num_Data(0, 2);
        LCD_Num_Data(0, 1);
    } else {
        for (uint8_t i = 0; i < InputData; i++) {
            LCD_Num_Data(CountBuf[i], (InputData - i));
        }
    }
    LCD_Temper_Sign;
#warning "LCD_缺少汉字进、回";
    return;
}

void ShowMeterID(void) {
    char InputDataBuff[8] = {0};
    int Buflen = shortChStrToDoubleChStr(NEW_NAME(AT24CXX_Manager_NET.MeterID), NEW_NAME(InputDataBuff));
    for (uint8_t i = 0; i < Buflen; i++) {
        LCD_Num_Data(InputDataBuff[i], (Buflen - i));
    }
    LCD_Meter_ID;
    return;
}

void ShowNowLimitTime(void) {
    TimeStuClass RTC_Date_SET = TimestampToRTCData(getTimeNumber_UTCByRTCTime(NEW_NAME(AT24CXX_Manager_NET.LimitTime)));
    LCD_Num_Data((20 % 100) / 10, 8);
    LCD_Num_Data((20 % 100) % 10, 7);
    LCD_Num_Data((RTC_Date_SET.year % 100) / 10, 6);
    LCD_Num_Data((RTC_Date_SET.year % 100) % 10, 5);
    LCD_SetDotPx(4, true);
    LCD_Num_Data((RTC_Date_SET.month % 100) / 10, 4);
    LCD_Num_Data((RTC_Date_SET.month % 100) % 10, 3);
#warning "LCD_第六个数码管缺少小数点";
    // LCD_SetDotPx(2, true);
    LCD_Num_Data((RTC_Date_SET.day % 100) / 10, 2);
    LCD_Num_Data((RTC_Date_SET.day % 100) % 10, 1);
    NO_LCD_ChineseOpen_VALVE;
    LCD_ChineseClose_VALVE;
    LCD_TimeDate;
    return;
}

void ShowRTCDate_Now(void) {
    FL_RTC_InitTypeDef RTC_Date_SET = RTC_Date;
    RTC_Date_SET.year = anyBaseToAnyBase(RTC_Date_SET.year, 10, 16);
    RTC_Date_SET.month = anyBaseToAnyBase(RTC_Date_SET.month, 10, 16);
    RTC_Date_SET.day = anyBaseToAnyBase(RTC_Date_SET.day, 10, 16);
    LCD_Num_Data((20 % 100) / 10, 8);
    LCD_Num_Data((20 % 100) % 10, 7);
    LCD_Num_Data((RTC_Date_SET.year % 100) / 10, 6);
    LCD_Num_Data((RTC_Date_SET.year % 100) % 10, 5);
    LCD_SetDotPx(4, true);
    LCD_Num_Data((RTC_Date_SET.month % 100) / 10, 4);
    LCD_Num_Data((RTC_Date_SET.month % 100) % 10, 3);
#warning "LCD_第六个数码管缺少小数点";
    // LCD_SetDotPx(2, true);
    LCD_Num_Data((RTC_Date_SET.day % 100) / 10, 2);
    LCD_Num_Data((RTC_Date_SET.day % 100) % 10, 1);
    NO_LCD_ChineseOpen_VALVE;
    NO_LCD_ChineseClose_VALVE;
    LCD_TimeDate;
    return;
}

void ShowRTCTime_Now(void) {
    FL_RTC_InitTypeDef RTC_Date_SET = RTC_Date;
    RTC_Date_SET.hour = anyBaseToAnyBase(RTC_Date_SET.hour, 10, 16);
    RTC_Date_SET.minute = anyBaseToAnyBase(RTC_Date_SET.minute, 10, 16);
    RTC_Date_SET.second = anyBaseToAnyBase(RTC_Date_SET.second, 10, 16);
    LCD_Num_Data((RTC_Date_SET.hour % 100) / 10, 8);
    LCD_Num_Data((RTC_Date_SET.hour % 100) % 10, 7);
    LCD_Num_Data(LINE2, 6);
    LCD_Num_Data((RTC_Date_SET.minute % 100) / 10, 5);
    LCD_Num_Data((RTC_Date_SET.minute % 100) % 10, 4);
    LCD_Num_Data(LINE2, 3);
    LCD_Num_Data((RTC_Date_SET.second % 100) / 10, 2);
    LCD_Num_Data((RTC_Date_SET.second % 100) % 10, 1);
#warning "LCD_Time";
    return;
}

void ShowUserEnableFlag(void) {
    unsigned char CountBuf[5] = {0};
    LCD_Num_Data(da_E, 8);
    LCD_Num_Data(da_N, 7);
    int Buflen = anyBaseNumberToAnyBaseArray(AT24CXX_Manager_NET.ModeCode, 16, 16, (char *)CountBuf, 5);
    if (Buflen <= 0) {
        LCD_Num_Data(0, 4);
        LCD_Num_Data(0, 3);
        LCD_Num_Data(0, 2);
        LCD_Num_Data(0, 1);
        return;
    }
    for (uint8_t i = 0; i < Buflen; i++) {
        LCD_Num_Data(CountBuf[i], (Buflen - i));
    }
}

// 显示
void ShowNBModeSignal_CSQ(void) {
    uint16_t InputData = AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_CSQ;
    unsigned char CountBuf[5] = {0};
    LCD_Num_Data(da_C, 8);
    LCD_Num_Data(da_S, 7);
    InputData = anyBaseNumberToAnyBaseArray((InputData / 10), 16, 10, (char *)CountBuf, 5);
    if ((int16_t)InputData <= 0) {
        LCD_Num_Data(0, 1);
    } else {
        for (uint8_t i = 0; i < InputData; i++) {
            LCD_Num_Data(CountBuf[i], (InputData - i));
        }
        if (InputData < 3) {
            LCD_Num_Data(0, 3);
        }
        if (InputData < 2) {
            LCD_Num_Data(0, 2);
        }
    }
    return;
}

void ShowBatV(void) {
    uint16_t InputData = System_RunData.Now_DEV_Volt;
    unsigned char CountBuf[5] = {0};
    LCD_Num_Data(da_U, 8);
    LCD_Num_Data(xiao_o, 7);
    LCD_Num_Data(da_I, 6);
    InputData = anyBaseNumberToAnyBaseArray((InputData / 10), 16, 10, (char *)CountBuf, 5);
    if ((int16_t)InputData <= 0) {
        LCD_Num_Data(0, 2);
        LCD_SetDotPx(1, true);
        LCD_Num_Data(0, 1);
    } else {
        for (uint8_t i = 0; i < InputData; i++) {
            LCD_Num_Data(CountBuf[i], (InputData - i));
        }
#warning "LCD_第六个数码管缺少小数点";
        // LCD_SetDotPx(2, true);
        if (InputData < 3) {
            LCD_Num_Data(0, 3);
        }
    }
    return;
}

void ShowVerNum(void) {
    uint8_t InputData = SOFT_VERSION;
    unsigned char CountBuf[3] = {0};
    LCD_Num_Data(da_U, 8);
    LCD_Num_Data(da_N, 7);
    InputData = anyBaseNumberToAnyBaseArray(InputData, 16, 10, (char *)CountBuf, 3);
    if ((int16_t)InputData <= 0) {
        LCD_Num_Data(0, 3);
        LCD_Num_Data(0, 2);
        LCD_Num_Data(0, 1);
    } else {
        for (uint8_t i = 0; i < InputData; i++) {
            LCD_Num_Data(CountBuf[i], (InputData - i));
        }
    }
    return;
}

void ShowErrerCode(void) {
    unsigned char CountBuf[5] = {0};
    LCD_Num_Data(da_E, 8);
    LCD_Num_Data(xiao_r, 7);
    int Buflen = anyBaseNumberToAnyBaseArray(System_RunData.StytemErrorCode, 16, 16, (char *)CountBuf, 5);
    if (Buflen <= 0) {
        LCD_Num_Data(0, 4);
        LCD_Num_Data(0, 3);
        LCD_Num_Data(0, 2);
        LCD_Num_Data(0, 1);
        return;
    }
    for (uint8_t i = 0; i < Buflen; i++) {
        LCD_Num_Data(CountBuf[i], (Buflen - i));
    }
    LCD_AlarmSign;
}
void ShowFrontValveCtrlUser(void) {
    uint8_t InputData = SetData_Input.DoneId;
    LCD_Num_Data(da_E, 8);
    LCD_Num_Data(2, 7);
    unsigned char CountBuf[3] = {0};
    InputData = anyBaseNumberToAnyBaseArray(InputData, 16, 10, (char *)CountBuf, 3);
    if ((int16_t)InputData <= 0) {
        LCD_Num_Data(0, 3);
        LCD_Num_Data(0, 2);
        LCD_Num_Data(0, 1);
    } else {
        for (uint8_t i = 0; i < InputData; i++) {
            LCD_Num_Data(CountBuf[i], (InputData - i));
        }
    }
    return;
}
void ShowIMEI(void) {
    ClearDisplayAll();
    newString(TempStr, 1);
    unsigned char a2, a3, a4, a5, a6, a7, a8;
    strnew Mother = NEW_NAME(AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_IMEI);
    if (MENU_ID == 12) {
        StringSlice(TempStr, Mother, 0, 1);
        a2 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 1, 2);
        a3 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 2, 3);
        a4 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 3, 4);
        a5 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 4, 5);
        a6 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 5, 6);
        a7 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 6, 7);
        a8 = (uint8_t)TempStr.Name._char[0] & 0x0F;
    } else {
        StringSlice(TempStr, Mother, 7, 8);
        a2 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 8, 9);
        a3 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 9, 10);
        a4 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 10, 11);
        a5 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 11, 12);
        a6 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 12, 13);
        a7 = (uint8_t)TempStr.Name._char[0] & 0x0F;
        StringSlice(TempStr, Mother, 13, 14);
        a8 = (uint8_t)TempStr.Name._char[0] & 0x0F;
    }
    LCD_Num_Data(da_E, 8);
    LCD_Num_Data(a2, 7);
    LCD_Num_Data(a3, 6);
    LCD_Num_Data(a4, 5);
    LCD_Num_Data(a5, 4);
    LCD_Num_Data(a6, 3);
    LCD_Num_Data(a7, 2);
    LCD_Num_Data(a8, 1);
    LcdDisplayRefresh();
}
void ShowIP(void) {
    unsigned char a1, a2, a3, a4, a5, a6, a7, a8;
    ClearDisplayAll();
    unsigned char EEprom_IP[4] = {0};
    IPstrToHexArray(NEW_NAME(EEprom_IP), AT24CXX_Manager_NET.NET_Remote_Url);
    a1 = EEprom_IP[0] / 0x10;
    a2 = EEprom_IP[0] % 0x10;
    a3 = EEprom_IP[1] / 0x10;
    a4 = EEprom_IP[1] % 0x10;
    a5 = EEprom_IP[2] / 0x10;
    a6 = EEprom_IP[2] % 0x10;
    a7 = EEprom_IP[3] / 0x10;
    a8 = EEprom_IP[3] % 0x10;

    LCD_Num_Data(a1, 8);
    LCD_Num_Data(a2, 7);
    LCD_Num_Data(a3, 6);
    LCD_Num_Data(a4, 5);
    LCD_Num_Data(a5, 4);
    LCD_Num_Data(a6, 3);
    LCD_Num_Data(a7, 2);
    LCD_Num_Data(a8, 1);
    LcdDisplayRefresh();
}
void ShowPort(void) {
    unsigned char a3, a4, a5, a6, a7;
    ClearDisplayAll();
    unsigned int Port = AT24CXX_Manager_NET.NET_Remote_Port;
    a3 = Port / 10000;
    a4 = (Port % 10000) / 1000;
    a5 = (Port % 1000) / 100;
    a6 = (Port % 100) / 10;
    a7 = Port % 10;

    LCD_Num_Data(da_P, 8);
    LCD_Num_Data(xiao_o, 7);
    LCD_Num_Data(xiao_r, 6);
    if (a3 != 0) {
        LCD_Num_Data(a3, 5);
    }
    LCD_Num_Data(a4, 4);
    LCD_Num_Data(a5, 3);
    LCD_Num_Data(a6, 2);
    LCD_Num_Data(a7, 1);
    LcdDisplayRefresh();
}
void Show_RSRP_RSRQ(void) {

    unsigned int RSP = AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_RSRP;
    unsigned int RSQ = AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_RSRQ;
    unsigned char a1, a2, a3, a5, a6, a7;
    ClearDisplayAll();
    a1 = (RSP % 1000) / 100;
    a2 = (RSP % 100) / 10;
    a3 = RSP % 10;

    a5 = (RSQ % 1000) / 100;
    a6 = (RSQ % 100) / 10;
    a7 = RSQ % 10;
    if (a1 != 0x00)LCD_Num_Data(a1, 8);
    if (a2 != 0x00)LCD_Num_Data(a2, 7);
    LCD_Num_Data(a3, 6);
    LCD_Num_Data(LINE2, 5);
    if (a5 != 0x00)LCD_Num_Data(a5, 4);
    if (a6 != 0x00)LCD_Num_Data(a6, 3);
    LCD_Num_Data(a7, 2);
    LCD_SINGAL;
    LcdDisplayRefresh();
}
void ShowCCID(void) {
    unsigned char EEprom_CCID[10] = {0};
    strnew CCIDStr = NEW_NAME(AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_ICCID);
    unsigned char a1, a2, a3, a4, a5, a6, a7;
    ClearDisplayAll();
    if (MENU_ID == 17) {
        a3 = EEprom_CCID[0] / 0x10;
        a4 = EEprom_CCID[0] % 0x10;
        a5 = EEprom_CCID[1] / 0x10;
        a6 = EEprom_CCID[1] % 0x10;
        a7 = EEprom_CCID[2] / 0x10;
        LCD_Num_Data(da_C, 7);
        LCD_Num_Data(da_D, 6);
        LCD_Num_Data(a3, 5);
        LCD_Num_Data(a4, 4);
        LCD_Num_Data(a5, 3);
        LCD_Num_Data(a6, 2);
        LCD_Num_Data(a7, 1);
    }
    if (MENU_ID == 18) {
        a1 = EEprom_CCID[2] % 0x10;
        a2 = EEprom_CCID[3] / 0x10;
        a3 = EEprom_CCID[3] % 0x10;
        a4 = EEprom_CCID[4] / 0x10;
        a5 = EEprom_CCID[4] % 0x10;
        a6 = EEprom_CCID[5] / 0x10;
        a7 = EEprom_CCID[5] % 0x10;
        LCD_Num_Data(a1, 7);
        LCD_Num_Data(a2, 6);
        LCD_Num_Data(a3, 5);
        LCD_Num_Data(a4, 4);
        LCD_Num_Data(a5, 3);
        LCD_Num_Data(a6, 2);
        LCD_Num_Data(a7, 1);
    }
    if (MENU_ID == 19) {
        a1 = EEprom_CCID[6] / 0x10;
        a2 = EEprom_CCID[6] % 0x10;
        a3 = EEprom_CCID[7] / 0x10;
        a4 = EEprom_CCID[7] % 0x10;
        a5 = EEprom_CCID[8] / 0x10;
        a6 = EEprom_CCID[8] % 0x10;
        a7 = EEprom_CCID[9] / 0x10;
        LCD_Num_Data(a1, 7);
        LCD_Num_Data(a2, 6);
        LCD_Num_Data(a3, 5);
        LCD_Num_Data(a4, 4);
        LCD_Num_Data(a5, 3);
        LCD_Num_Data(a6, 2);
        LCD_Num_Data(a7, 1);
    }
    LcdDisplayRefresh();
}
void background(void) {
    if (System_RunData.Now_NetDevParameter.isSendOk) {
        LCD_SINGAL;
    } else {
        NO_LCD_SINGAL;
    }
    {
        uint16_t vol = System_RunData.Now_DEV_Volt / 10;
        LCD_BATT_BACK;
        if (vol >= 345) {
            LCD_BATTERYX1;
            LCD_BATTERYX2;
            LCD_BATTERYX3;
            LCD_BATTERYX4;
        } else if ((vol >= 340) && (vol < 345)) {
            LCD_BATTERYX1;
            LCD_BATTERYX2;
            LCD_BATTERYX3;
        } else if ((vol >= 330) && (vol < 340)) {
            LCD_BATTERYX1;
            LCD_BATTERYX2;
        } else if ((vol >= 310) && (vol < 330)) {
            LCD_BATTERYX1;
        }
    }
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableTimeLimit)) {
#warning "LCD_PleasePay";
    }
    if (((unsigned char)UpdataData.Sign == 0xB2) && (System_RunData.isUpCode == false)) {
        UpdataData.Sign = 0;
        UpdataData.NowPageNum = 0;
        UpdataData.NowLen_Page8Buff = 0;
    }
    if (System_RunData.isUpCode == true) {
        System_RunData.isUpCode = false;
        Show_UpCode_Start();
    }
}

void LCD_MenuPage_Change(char UserPage) {
    if ((UserPage != '\0') && (UserPage < '9') && (UserPage > '0')) {
        MENU_ID = UserPage - '0';
        MENU_ID = (MENU_ID > 0 ? MENU_ID - 1 : MaxMenuNum - 1);
        RTC_TASK.InitSetTimeTask(HomePageRefresh, 30, ShowHomePage);
        return;
    }
    ClearDisplayAll();
    background();
    Display MenuItme[MaxMenuNum] = {
        ShowValvePart,
        ShowMeterID,
        ShowNowLimitTime,
        ShowTemper,
        ShowRTCDate_Now,
        ShowRTCTime_Now,
        ShowUserEnableFlag,
        ShowNBModeSignal_CSQ,
        ShowBatV,
        ShowVerNum,
        ShowErrerCode,
        ShowFrontValveCtrlUser,
        ShowIMEI,
        ShowIMEI,
        ShowIP,
        ShowPort,
        Show_RSRP_RSRQ,
        ShowCCID,
        ShowCCID,
        ShowCCID,
    };
    MENU_ID = (MENU_ID < (MaxMenuNum - 1) ? MENU_ID + 1 : 0);
    MenuItme[MENU_ID]();
    LcdDisplayRefresh();
}

void ShowHomePage(void) {
    RTC_TASK.InitSetTimeTask(HomePageRefresh, 30, ShowHomePage);
    MENU_ID = 0;
    ClearDisplayAll();
    background();
    ShowValvePart();
    LcdDisplayRefresh();
    return;
}

void ShowClosePage(void) {
    ClearDisplayAll();
    background();
    LCD_ChineseClose_VALVE;
    NO_LCD_ChineseOpen_VALVE;
    LCD_Num_Data(LINE2, 8);
    LCD_Num_Data(LINE2, 7);
    LCD_Num_Data(da_C, 6);
    LCD_Num_Data(da_L, 5);
    LCD_Num_Data(da_O, 4);
    LCD_Num_Data(da_S, 3);
    LCD_Num_Data(da_E, 2);
    LCD_Num_Data(LINE2, 1);
    LcdDisplayRefresh();
    return;
}

void ShowOpenPage(void) {
    ClearDisplayAll();
    background();
    NO_LCD_ChineseClose_VALVE;
    LCD_ChineseOpen_VALVE;
    LCD_Num_Data(LINE2, 8);
    LCD_Num_Data(LINE2, 7);
    LCD_Num_Data(da_O, 6);
    LCD_Num_Data(da_P, 5);
    LCD_Num_Data(da_E, 4);
    LCD_Num_Data(da_N, 3);
    LCD_Num_Data(LINE2, 2);
    LCD_Num_Data(LINE2, 1);
    LcdDisplayRefresh();
    return;
}

void ShowSignal(void) {
    if (!System_RunData.ReceiveFlag) {
        return;
    }
    LCD_SetDotPx(1, true);
    LcdDisplayRefresh();
    return;
}

void ShowWait(bool isWriteEeprom) {
    ClearDisplayAll();
    if (isWriteEeprom) {
        LCD_Num_Data(da_E, 8);
    }
    LCD_Num_Data(LINE2, 6);
    LCD_Num_Data(LINE2, 5);
    LCD_Num_Data(LINE2, 4);
    LCD_Num_Data(LINE2, 3);
    LcdDisplayRefresh();
    return;
}

///////////////////////////////////
// 显示错误码   阀门
void Show_M1_Check_ErrerCode(uint8_t ShowID) {
    ShowID = (ShowID < 3 ? ShowID : 2);
    ClearDisplayAll();
    if ((++ShowID) == 3) {
        LCD_Meter_ID;
    }
    background();
    LCD_Num_Data(LINE2, 8);
    LCD_Num_Data(LINE2, 7);
    LCD_Num_Data(da_E, 6);
    LCD_Num_Data(xiao_r, 5);
    LCD_Num_Data(xiao_r, 4);
    LCD_Num_Data(LINE2, 3);
    LCD_Num_Data(LINE2, 2);
    LCD_Num_Data(ShowID, 1);
    LcdDisplayRefresh();
    return;
}
// 显示升级
void Show_UpCode_Start(void) {
    LCD_Num_Data(da_U, 7);
    LCD_Num_Data(da_P, 6);
    LcdDisplayRefresh();
    return;
}
// 测试 LCD
void TesTLCD(void) {
    for (uint8_t i = 0; i < 10; i++) {
        ClearDisplayAll();
        switch (i) {
            case 0:
                LCD_ChineseOpen_VALVE;
                LCD_ChineseClose_VALVE;
                break;
            case 1:
                LCD_TimeDate;
                LCD_Meter_ID;
                break;
            case 2:
                LCD_CAIJIQI_ID;
                LCD_ChineseRest;
                break;
            case 3:
                LCD_BATTERYX1;
                LCD_BATTERYX2;
                break;
            case 4:
                LCD_AlarmSign;
                LCD_SINGAL;
                break;
            case 5:
                LCD_Temper_Sign;
                LCD_GJ_Sign;
                break;
            case 6:
                LCD_Kw_Hour_Sign;
                LCD_Kw_Sign;
                break;
            case 7:
                LCD_BATT_BACK;
                LCD_BATTERYX4;
                break;
            case 8:
                LCD_BATTERYX3;
            default:
                // LCD_ChineseRoomTemper;
                // LCD_PleasePay;
                // LCD_ChineseHeat;
                // LCD_ChineseSet;
                // LCD_ChineseTemper;
                // LCD_COL;
                // LCD_Time;
                break;
        }
        LCD_Num_Data(i, 8);
        LCD_Num_Data(i, 7);
        LCD_Num_Data(i, 6);
        LCD_Num_Data(i, 5);
        LCD_Num_Data(i, 4);
        LCD_Num_Data(i, 3);
        LCD_Num_Data(i, 2);
        LCD_Num_Data(i, 1);
        LcdDisplayRefresh();
        IncludeDelayMs(300);
    }
}
void Display__Module_Steep(unsigned char steep, unsigned char status) {
    ClearDisplayAll();
    LCD_Num_Data(0x07, 8);
    LCD_Num_Data(xiao_o, 7);
    LCD_Num_Data(LINE2, 6);
    if (steep != 255) {
        LCD_Num_Data(steep / 10, 5);
        LCD_Num_Data(steep % 10, 4);
    } else {
        LCD_Num_Data(da_N, 5);
        LCD_Num_Data(da_E, 4);
    }

    LCD_Num_Data(LINE2, 3);
    if (status == 0) {
        LCD_Num_Data(da_N, 2);
        LCD_Num_Data(xiao_o, 1);
    } else {
        LCD_Num_Data(xiao_o, 2);
        LCD_Num_Data(da_H, 1);
    }
    ShowSignal();
    LcdDisplayRefresh();
}


