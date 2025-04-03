#include "AT24CXXDataLoader.h"
#include "stddef.h"

struct AT24CXX_MANAGER_S * _AT24CXX_Manager_NET = NULL;
#ifndef __AT24C0XXX_H
AT24CXX_MANAGER_T AT24CXX_Manager_NET;
#endif
void AT24CXXLoader_Init(void) {
    // AT24CXX_Manager.SendManageObj.SendIntervalDay = MinToSec(HourToMin(6));
    _AT24CXX_Manager_NET = &AT24CXX_Manager;
    AT24CXX_Manager_NET.NowRunMode = RunModeOfSystemStart; // 0：关机 1：自动（默认模式） 2：速热 3：时段 4：防冻
    AT24CXX_Manager_NET.Time_Data.year = 0x19;
    AT24CXX_Manager_NET.Time_Data.month = 0x10;
    AT24CXX_Manager_NET.Time_Data.day = 0x01;
    AT24CXX_Manager_NET.Time_Data.hour = 0x06;
    AT24CXX_Manager_NET.Time_Data.minute = 0x30;
    AT24CXX_Manager_NET.Time_Data.second = 0x59;
    AT24CXX_Manager_NET.Time_Data.week = getDayOfWeek(0x2020, 0x01, 0x01);
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
// #ifdef _Module_EEpromTime
// #include "NetProt_Module.h"
// void setIsWriteEEprom(uint8_t UserFlagType) {
//     // 输入 false
//     if (UserFlagType == false) {
//         Now_NetDevParameter.isWriteEEprom = false;
//         return;
//     }
//     // 输入非 false, 原始值是 false 直接修改
//     if (Now_NetDevParameter.isWriteEEprom == false) {
//         Now_NetDevParameter.isWriteEEprom = UserFlagType;   // UserFlagType != false
//         return;
//     }

//     // 输入非 false, 原始值已被修改过了, 当前值为 _Module_EEpromSend 开启重要上报并写eeprom 不能被替换
//     if (Now_NetDevParameter.isWriteEEprom == _Module_EEpromSend) { // _Module_EEpromSend 优先级 No.1
//         return;
//     }
//     // 输入非 false, 原始值已被修改过了, 当前值为 true 可被 _Module_EEpromSend 替换
//     if (Now_NetDevParameter.isWriteEEprom == true) {  // true 优先级 No.2
//         Now_NetDevParameter.isWriteEEprom = (UserFlagType == _Module_EEpromSend ? _Module_EEpromSend : true);
//         return;
//     }
//     // 输入非 false, 原始值已被修改过了, 当前值为 _Module_EEpromTime 可被 true 和 _Module_EEpromSend 替换
//     if (Now_NetDevParameter.isWriteEEprom == _Module_EEpromTime) {  // true 优先级 No.3
//         if (UserFlagType == true || UserFlagType == _Module_EEpromSend) {
//             Now_NetDevParameter.isWriteEEprom = UserFlagType;
//         }
//         return;
//     }

//     if (UserFlagType == _Module_EEpromTime || UserFlagType == true || UserFlagType == _Module_EEpromSend) {
//         Now_NetDevParameter.isWriteEEprom = UserFlagType;   // UserFlagType != false
//     }
//     Now_NetDevParameter.isWriteEEprom = false;
//     return;
// }
// #endif
#include "../Interflow/StrLib.h"
strnew UartBuff = {0};    // Uart接收缓冲区
// NetDevParameter Now_NetDevParameter;    // 网络状态标记与下行指令表

// AT 参数初始化
// 初始化 Now_NetDevParameter
void setNetArgumentInit(void (*UserShowdownNowDev)(void)) {
    // Now_NetDevParameter.CheckOnlineFlag = setDataBit(Now_NetDevParameter.CheckOnlineFlag, 4, false); // 是否需要被动监控某个字段
    // Now_NetDevParameter.LineCheckTime = 5 * 60;     // 判断你是否处于连接状态
    // Now_NetDevParameter.ShowdownNowDev = UserShowdownNowDev;
    // Now_NetDevParameter.SendData = UserSendData;    // 长连接不需要发送函数
    // Now_NetDevParameter.DoneCmd = UserDoneCmd;      // 处理指令
    // Now_NetDevParameter.GetDownCmd = UserGetDownCmd;// 获取下行指令
    // Now_NetDevParameter.isGetDownCmd = false;       // 是否主动获取指令
    // Now_NetDevParameter.checkFlagStr = NULL;        // 主动获取指令时模组主动推出的提示字符串
    // TableOfCmdTaskInit();                           // 指令表初始化
    UartBuff = NEW_NAME(UART_DATABUFF);             // 初始化缓存 UartBuff
    // 初始化 SetTime 任务
    // 初始化 SetLPTime 任务
    // SetLPTime.LPInitSetTimeTask(HomePage, SecTo250Ms(0.9), ShowHomePage);    //1s 执行一次
    // 初始化 RTC_TASK 任务  
    #warning need check; 
    RTC_TASK.InitSetTimeTask(SendIntervalTask, MinToSec(8), NULL);
    RTC_TASK.InitSetTimeTask(DayOverCclkTask, MinToSec(HourToMin(AT24CXX_Manager_NET.DaysNumberOfCCLK * 24)), NULL); // 604,800
    RTC_TASK.InitSetTimeTask(EEpromWriteTime, MinToSec(15), NULL);  // 15 分钟写入一次 EEPROM Time
    RTC_TASK.InitSetTimeTask(BatVoltge, MinToSec(1), NULL); // 1 分钟检测一次电池电压
    RTC_TASK.InitSetTimeTask(InsideT, MinToSec(1), NULL); // 1 分钟检测一次内部温度
    RTC_TASK.InitSetTimeTask(Read_EEprom, MinToSec(HourToMin(6)), NULL);
    return;
}

