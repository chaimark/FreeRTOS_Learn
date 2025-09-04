#include "AT24CXXDataLoader.h"
#include "stddef.h"
#include "StrLib.h"
#include "Display.h"

struct AT24CXX_MANAGER_S * _AT24CXX_Manager_NET = NULL;
#ifndef __AT24C0XXX_H
AT24CXX_MANAGER_T AT24CXX_Manager_NET;
#endif
void AT24CXXLoader_Init(void) {
    _AT24CXX_Manager_NET = &AT24CXX_Manager;
    AT24CXX_Manager_NET.Time_Data.year = 0x19;
    AT24CXX_Manager_NET.Time_Data.month = 0x10;
    AT24CXX_Manager_NET.Time_Data.day = 0x01;
    AT24CXX_Manager_NET.Time_Data.hour = 0x06;
    AT24CXX_Manager_NET.Time_Data.minute = 0x30;
    AT24CXX_Manager_NET.Time_Data.second = 0x59;
    AT24CXX_Manager_NET.Time_Data.week = getDayOfWeek(0x2020, 0x01, 0x01);
    AT24CXX_Manager_NET.ModeCode = 0xFFF;
}
bool checkTimeFrom(FL_RTC_InitTypeDef InputTimeData) {
    if (!Check_Time_ByHEX(InputTimeData.hour, InputTimeData.minute, InputTimeData.second)) {
        return false;    // 时间错误
    }
    if (!Check_Date_ByHEX((InputTimeData.year >> 16), InputTimeData.year, InputTimeData.month, InputTimeData.day)) {
        return false;    // 时间错误
    }
    return ((InputTimeData.year & 0x00FF) < 0x0019 ? false : true);
}
#ifdef _Module_EEpromTime
void setIsWriteEEprom(uint8_t UserFlagType) {
    if (UserFlagType == 0xFF) {
        return;
    }
    // 输入 false
    if (UserFlagType == false) {
        System_RunData.Now_NetDevParameter.isWriteEEprom = false;
        return;
    }
    // 输入非 false, 原始值是 false 直接修改
    if (System_RunData.Now_NetDevParameter.isWriteEEprom == false) {
        System_RunData.Now_NetDevParameter.isWriteEEprom = UserFlagType;   // UserFlagType != false
        return;
    }

    // 输入非 false, 原始值已被修改过了, 当前值为 _Module_EEpromSend 开启重要上报并写eeprom 不能被替换
    if (System_RunData.Now_NetDevParameter.isWriteEEprom == _Module_EEpromSend) { // _Module_EEpromSend 优先级 No.1
        return;
    }
    // 输入非 false, 原始值已被修改过了, 当前值为 true 可被 _Module_EEpromSend 替换
    if (System_RunData.Now_NetDevParameter.isWriteEEprom == true) {  // true 优先级 No.2
        System_RunData.Now_NetDevParameter.isWriteEEprom = (UserFlagType == _Module_EEpromSend ? _Module_EEpromSend : true);
        return;
    }
    // 输入非 false, 原始值已被修改过了, 当前值为 _Module_EEpromTime 可被 true 和 _Module_EEpromSend 替换
    if (System_RunData.Now_NetDevParameter.isWriteEEprom == _Module_EEpromTime) {  // true 优先级 No.3
        if (UserFlagType == true || UserFlagType == _Module_EEpromSend) {
            System_RunData.Now_NetDevParameter.isWriteEEprom = UserFlagType;
        }
        return;
    }

    if (UserFlagType == _Module_EEpromTime || UserFlagType == true || UserFlagType == _Module_EEpromSend) {
        System_RunData.Now_NetDevParameter.isWriteEEprom = UserFlagType;   // UserFlagType != false
        return;
    }
    System_RunData.Now_NetDevParameter.isWriteEEprom = false;
    return;
}
#endif

// AT 参数初始化
// 初始化 Get_Module_Data
void setNetArgumentInit(void (*UserShowdownNowDev)(void)) {
    // 初始化 SetTime 任务
    // 初始化 SetLPTime 任务
    // 初始化 RTC_TASK 任务 
    RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
#warning "调试用的临时权限";
    AT24CXX_Manager_NET.ModeCode = 0;
    AT24CXX_Manager_NET.ModeCode = setDataBit(AT24CXX_Manager_NET.ModeCode, EnableADCTestTemper, true);
    // AT24CXX_Manager_NET.ModeCode = setDataBit(AT24CXX_Manager_NET.ModeCode, EnableGP2TestTemper, true);
    AT24CXX_Manager_NET.ModeCode = setDataBit(AT24CXX_Manager_NET.ModeCode, EnableADCTestValve, true);
    // AT24CXX_Manager_NET.ModeCode = setDataBit(AT24CXX_Manager_NET.ModeCode, SaveRunTimeForOpenValve, true);
    AT24CXX_Manager_NET.ModeCode = setDataBit(AT24CXX_Manager_NET.ModeCode, EnableM1Card, true);
    // AT24CXX_Manager_NET.ModeCode = setDataBit(AT24CXX_Manager_NET.ModeCode, EnableTimeLimit, true);
    // AT24CXX_Manager_NET.ModeCode = setDataBit(AT24CXX_Manager_NET.ModeCode, ValveAutoTest, true);
    return;
}

