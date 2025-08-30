#include <string.h>
#include "fm33lc0xx_fl.h"
#include "Display.h"
#include "Main.h"
#include "RTC.h"
#include "../Interflow/PublicLib_No_One.h"
#include "../Interflow/NumberBaseLib.h"
#include "../Interflow/StrLib.h"
#include "../Interflow/AT24CXXDataLoader.h"

/*********************Menu***************************************************************/
#define MaxMenuNum 12
typedef void (*Display)(void * _IntputData);

void ShowValvePart(void * _IntputData) {
    uint16_t InputData = Get_Module_Data.Now_Degree_Part;
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, Enable433Open)) {
        LCD_Num_Data(LINE2, 8);
    }
    unsigned char CountBuf[4] = {0};
    InputData = anyBaseNumberToAnyBaseArray(InputData, 16, 10, (char *)CountBuf, 4);
    LCD_SetDotPx(1);
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

void ShowTemper(void * _IntputData) {
    unsigned char CountBuf[4] = {0};
    int InputData = floatToUint6410Rate(Get_Module_Data.Now_Temper_T1);
    InputData = anyBaseNumberToAnyBaseArray(InputData, 16, 10, (char *)CountBuf, 4);
    LCD_SetDotPx(5);
    if (InputData <= 0) {
        LCD_Num_Data(0, 6);
        LCD_Num_Data(0, 5);
    } else {
        for (uint8_t i = 0; i < InputData; i++) {
            LCD_Num_Data(CountBuf[i], (InputData - i + 4));
        }
    }
    InputData = floatToUint6410Rate(Get_Module_Data.Now_Temper_T2);
    InputData = anyBaseNumberToAnyBaseArray(InputData, 16, 10, (char *)CountBuf, 4);
    LCD_SetDotPx(1);
    if (InputData <= 0) {
        LCD_Num_Data(0, 2);
        LCD_Num_Data(0, 1);
    } else {
        for (uint8_t i = 0; i < InputData; i++) {
            LCD_Num_Data(CountBuf[i], (InputData - i));
        }
    }
    LCD_Temper_Sign;
    return;
}
// T2
// void ShowTemper(void * _IntputData) {
//     char InputData = Get_Module_Data.Now_Temper_T1;
//     // LCD_Num_Data();
//     return;
// }

void ShowMeterID(void * _IntputData) {
    char InputDataBuff[8] = {0};
    int Buflen = shortChStrToDoubleChStr(NEW_NAME(AT24CXX_Manager_NET.MeterID), NEW_NAME(InputDataBuff));
    for (uint8_t i = 0; i < Buflen; i++) {
        LCD_Num_Data(InputDataBuff[i], (Buflen - i));
    }
    LCD_Meter_ID;
    return;
}

#include <time.h>
// 假设输入的时间戳为UTC秒数
FL_RTC_InitTypeDef TimestampToRTCData(uint32_t timestamp) {
    FL_RTC_InitTypeDef TempRTCData = {0};
    // 使用标准库的时间结构体
    time_t rawtime = (time_t)timestamp;
    struct tm * timeinfo = gmtime(&rawtime);

    if (timeinfo) {
        TempRTCData.year = timeinfo->tm_year + 1900;
        TempRTCData.month = timeinfo->tm_mon + 1;
        TempRTCData.day = timeinfo->tm_mday;
        TempRTCData.week = timeinfo->tm_wday;
        TempRTCData.hour = timeinfo->tm_hour;
        TempRTCData.minute = timeinfo->tm_min;
        TempRTCData.second = timeinfo->tm_sec;
    }
    return TempRTCData;
}

void ShowNowLimitTime(void * _IntputData) {
    FL_RTC_InitTypeDef RTC_Date_SET = TimestampToRTCData(getTimeNumber_UTCByRTCTime(NEW_NAME(AT24CXX_Manager_NET.LimitTime)));
    LCD_Num_Data((RTC_Date_SET.year % 100) / 10, 8);
    LCD_Num_Data((RTC_Date_SET.year % 100) % 10, 7);
    LCD_Num_Data(LINE2, 6);
    LCD_Num_Data((RTC_Date_SET.month % 100) / 10, 5);
    LCD_Num_Data((RTC_Date_SET.month % 100) % 10, 3);
    LCD_Num_Data(LINE2, 3);
    LCD_Num_Data((RTC_Date_SET.day % 100) / 10, 2);
    LCD_Num_Data((RTC_Date_SET.day % 100) % 10, 1);
    return;
}

void ShowRTCDate_Now(void * _IntputData) {
    LCD_Num_Data((RTC_Date.year % 100) / 10, 8);
    LCD_Num_Data((RTC_Date.year % 100) % 10, 7);
    LCD_Num_Data(LINE2, 6);
    LCD_Num_Data((RTC_Date.month % 100) / 10, 5);
    LCD_Num_Data((RTC_Date.month % 100) % 10, 3);
    LCD_Num_Data(LINE2, 3);
    LCD_Num_Data((RTC_Date.day % 100) / 10, 2);
    LCD_Num_Data((RTC_Date.day % 100) % 10, 1);
    LCD_TimeDate;
    return;
}

void ShowRTCTime_Now(void * _IntputData) {
    LCD_Num_Data((RTC_Date.hour % 100) / 10, 8);
    LCD_Num_Data((RTC_Date.hour % 100) % 10, 7);
    LCD_Num_Data(LINE2, 6);
    LCD_Num_Data((RTC_Date.minute % 100) / 10, 5);
    LCD_Num_Data((RTC_Date.minute % 100) % 10, 3);
    LCD_Num_Data(LINE2, 3);
    LCD_Num_Data((RTC_Date.second % 100) / 10, 2);
    LCD_Num_Data((RTC_Date.second % 100) % 10, 1);
    LCD_Time;
    return;
}

void ShowUserEnableFlag(void * _IntputData) {
    unsigned char CountBuf[4] = {0};
    int Buflen = anyBaseNumberToAnyBaseArray(AT24CXX_Manager_NET.ModeCode, 16, 16, (char *)CountBuf, 4);
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

void ShowUserCtrlDevData(void * _IntputData) {
    // 显示用户室内温度和设定温度标记
    uint16_t InputData = Get_Module_Data.CtrlDev_TemperSet;
    unsigned char CountBuf[4] = {0};
    InputData = anyBaseNumberToAnyBaseArray(InputData, 16, 10, (char *)CountBuf, 4);
    LCD_SetDotPx(5);
    if ((int16_t)InputData <= 0) {
        LCD_Num_Data(0, 6);
        LCD_Num_Data(0, 5);
    } else {
        for (uint8_t i = 0; i < InputData; i++) {
            LCD_Num_Data(CountBuf[i], (InputData - i + 4));
        }
    }
    LCD_ChineseSet;
    LCD_ChineseTemper;
    InputData = Get_Module_Data.CtrlDev_TemperRoom;
    LCD_SetDotPx(1);
    if ((int16_t)InputData <= 0) {
        LCD_Num_Data(0, 2);
        LCD_Num_Data(0, 1);
    } else {
        for (uint8_t i = 0; i < InputData; i++) {
            LCD_Num_Data(CountBuf[i], (InputData - i));
        }
    }
    LCD_ChineseRoomTemper;
    return;
}

void ShowBatV(void * _IntputData) {
    uint16_t InputData = Get_Module_Data.Now_DEV_Volt;
    unsigned char CountBuf[4] = {0};
    InputData = anyBaseNumberToAnyBaseArray(InputData, 16, 10, (char *)CountBuf, 4);
    LCD_SetDotPx(1);
    if ((int16_t)InputData <= 0) {
        LCD_Num_Data(0, 2);
        LCD_Num_Data(0, 1);
    } else {
        for (uint8_t i = 0; i < InputData; i++) {
            LCD_Num_Data(CountBuf[i], (InputData - i));
        }
    }
    return;
}

void ShowVerNum(void * _IntputData) {
    uint8_t InputData = SOFT_VERSION;
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

void ShowErrerCode(void * _IntputData) {
    unsigned char CountBuf[4] = {0};
    int Buflen = anyBaseNumberToAnyBaseArray(Get_Module_Data.StytemErrorCode, 16, 16, (char *)CountBuf, 4);
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
#include "Task_Valve.h"
void ShowFrontValveCtrlUser(void * _IntputData) {
    uint8_t InputData = SetData_Input.DoneId;
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

void background(void) {
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableTestBatV)) {
        uint16_t vol = Get_Module_Data.Now_DEV_Volt;
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
        LCD_PleasePay; // 显示请购买
    }
    if (Get_Module_Data.Now_Degree_Part <= 50) {
        LCD_ChineseClose_VALVE;
    } else if (Get_Module_Data.Now_Degree_Part >= 950) {
        LCD_ChineseOpen_VALVE;
    } else {
        SetErrerCode(CODE_ERR_VALVE_OPEN_FAULT, true);
    }
}

void LCD_MenuPage_Change(void) {
    static uint8_t MENU_ID = 0;
    Display MenuItme[MaxMenuNum] = {
        ShowValvePart,
        ShowTemper,
        ShowMeterID,
        ShowNowLimitTime,
        ShowRTCDate_Now,
        ShowRTCTime_Now,
        ShowUserEnableFlag,
        ShowUserCtrlDevData,
        ShowBatV,
        ShowVerNum,
        ShowErrerCode,
        ShowFrontValveCtrlUser
    };
    ClearDisplayAll();
    background();
    MENU_ID = (MENU_ID < (MaxMenuNum - 1) ? MENU_ID + 1 : 0);
    MenuItme[MENU_ID](NULL);
    LcdDisplayRefresh();
}

void ShowHomePage(void) {
    RTC_TASK.InitSetTimeTask(HomePageRefresh, 1, ShowHomePage);
    ClearDisplayAll();
    background();
    ShowValvePart(NULL);
    LcdDisplayRefresh();
    return;
}

void ShowClosePage(void) {
    ClearDisplayAll();
    background();
    LCD_Num_Data(LINE2, 8);
    LCD_Num_Data(LINE2, 7);
    LCD_Num_Data(da_C, 6);
    LCD_Num_Data(da_L, 5);
    LCD_Num_Data(da_O, 4);
    LCD_Num_Data(da_S, 3);
    LCD_Num_Data(da_S, 2);
    LCD_Num_Data(LINE2, 1);
    LcdDisplayRefresh();
    return;
}

void ShowOpenPage(void) {
    ClearDisplayAll();
    background();
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

void ShowSignal(bool IsLinght) {
    if (IsLinght) {
        LCD_SINGAL;
    } else {
        NO_LCD_SINGAL;
    }
    LcdDisplayRefresh();
    return;
}

void ShowWait(void) {
    ClearDisplayAll();
    LCD_Num_Data(LINE2, 3);
    LCD_Num_Data(LINE2, 4);
    LCD_Num_Data(LINE2, 5);
    LCD_Num_Data(LINE2, 6);
    LcdDisplayRefresh();
    return;
}

