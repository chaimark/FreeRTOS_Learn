#include "../Interflow/Define.h"

const char SOFT_VERSION = 11; // 版本号
unsigned int UART_BAND = 115200;

LongUART UART1Ddata;
///////////////////////
ShortUART LPUART0Ddata;

unsigned int Now_DEV_Volt = 3600;
unsigned int MENU_Id = 0;

void Device_Init(void) {
    #warning need check;
    // MF_UART1_Init();    // 初始化 Uart1
    // MF_LPUART0_Init();  // 初始化 LPUart0
    // Display_Wait(0);
    // RTC 开启定时器中断
    // MF_RTC_1S_Init();
    // LPTIM 开启定时器中断
    // MF_LPTIM250mS_Init();
    // // BSTIM 开启定时器中断
    // BSTIM32_Setup();    //开启定时器中断
    // BSTIM32_Stop();     //关闭定时器中断
    // MF_LPTIM250mS_Init();
    SetLPTime.LPInitSetTimeTask(IWDTClS, SecTo250Ms(MinToSec(8)), NULL); // 8min 内定时器喂狗
}
void StartOPenDevMode(void) {
    #warning need check;
    Device_Init();
//     /* Initial IWDT */
//     MF_IWDT_Init();
//     MF_EXTI_Line_Init(); // 配置外部中断对应的端口

//     FL_RTC_InitTypeDef TempTime;
// #ifdef OPEN8025T
//     TempTime = Read_RX8025T_Time();
//     RTC_SetRTC(&TempTime);    // 设置时间结构体
//     TempTime = RTC_Date;
//     Write_RX8025T_Time(TempTime);
//     RTC_TASK.InitSetTimeTask(__RX8025T_H, MinToSec(HourToMin(1)), NULL); // 每小时校准 RTC
// #else
//     ReadTime_EEprom(&TempTime);
//     RTC_SetRTC(&TempTime);    // 设置时间结构体
// #endif
//     IncludeDelayMs(1000);

//     // MF_UART1_Interrupt_Init();      // 开启 Uart1
//     MF_LPUART0_Interrupt_Init();    // 开启 LPUart0
}
void CheckMeterNum(void) {
    // FL_RTC_InitTypeDef TempTime;
    // ReadTime_EEprom(&TempTime);
    // char setStrOfTime[32] = {0};
    // strnew NowTimeData = NEW_NAME(setStrOfTime);
    // sprintf(NowTimeData.Name._char, "%02x-%02x-%02x %02x:%02x:%02x", TempTime.year, TempTime.month, TempTime.day, TempTime.hour, TempTime.minute, TempTime.second);
    // setRtcDate(NowTimeData, false); // 设置时间
    // AT24CXX_Manager_NET.NowRunMode = (AT24CXX_Manager_NET.UserModeFlag ? AT24CXX_Manager_NET.NowRunMode : RunModeOfSystemStart);
    // changeModeForT();
}
