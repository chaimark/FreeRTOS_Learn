#include "TaskEEprom.h"
#include "PublicLib_No_One.h"
#include "AT24CXXDataLoader.h"
#include "RTC_SetTime.h"
#include "StrLib.h"
#include "timers.h"
#include "event_groups.h"
#include "Task_Valve.h"
#include "Display.h"
#include "NB_Mode_Lib.h"

SemaphoreHandle_t EEprom_RunFlag = NULL;
void isRunEEprom(void) {
    // 获取互斥量，防止读写冲突
    while (!(xSemaphoreTake(EEprom_RunFlag, 100) == pdTRUE)) {
        vTaskDelay(100);
    }
}
void init_EEprom_RunFlag(void) {
    static bool isInit_RunFlag = false;
    if (isInit_RunFlag) {
        return;
    }
    EEprom_RunFlag = xSemaphoreCreateBinary(); // 初始化信号量
    xSemaphoreGive(EEprom_RunFlag);
}
//////////////////////////////////////////////////////////////
TaskHandle_t ReadEEpromHand = NULL;
SemaphoreHandle_t ReadEEpromSemaphore = NULL;
void ReadEEpromTimerCallback(void) {
    MIN_TASK.InitSetTimeTask(TaskReadEEprom, HourToMin(1), ReadEEpromTimerCallback);
    xSemaphoreGive(ReadEEpromSemaphore);
}
void ReadEEprom(void * pvParameters) {
    MIN_TASK.InitSetTimeTask(TaskReadEEprom, HourToMin(1), ReadEEpromTimerCallback);
    ReadEEpromSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    init_EEprom_RunFlag();
    while (1) {
        // 等待信号量阻塞任务, 转为运行其他任务
        if (xSemaphoreTake(ReadEEpromSemaphore, portMAX_DELAY) == pdTRUE) {
            isRunEEprom();
            EEprom_AT24CXX_Parameter_Init(false);
            clearEEpromFlag;
        }
    }
}
//////////////////////////////////////////////////////////////
void WriteRunTime(uint16_t backNum) {
    uint32_t RunTimeNum = 0;
    EEprom_Read_Start((unsigned char *)&RunTimeNum, EEPROM_OTHER_ADDRESS + (backNum * 4), 4);
    RunTimeNum += System_RunData.TempRunTimeForValve; // 已运行 System_RunData.TempRunTimeForValve 分钟
    System_RunData.TempRunTimeForValve = 0;
    if (RunTimeNum > 150000 * (backNum + 1)) {    // 是否需要切换
        EEprom_Write_Start((unsigned char *)&RunTimeNum, EEPROM_OTHER_ADDRESS + (backNum + 1 * 4), 4);
        AT24CXX_Manager_NET.RunTimeBack_AddrNum++;
        EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.RunTimeBack_AddrNum, sizeof(AT24CXX_Manager_NET.RunTimeBack_AddrNum));
    } else {    // 写阀门运行时间
        EEprom_Write_Start((unsigned char *)&RunTimeNum, EEPROM_OTHER_ADDRESS + (backNum * 4), 4);
    }
}

EventGroupHandle_t xEventGroup_EEprom = NULL;
#define WriteNowRTCTime     (1 << 0)
#define WriteValveRunTime   (1 << 1)
#define WriteAT24C0xxData   (1 << 2)
#define AllxEvent_EEprom (WriteNowRTCTime | WriteValveRunTime | WriteAT24C0xxData)

TaskHandle_t WriteEEpromHand = NULL;
SemaphoreHandle_t WriteEEpromSemaphore = NULL;
void WriteEEpromTimerCallback(void) {
    MIN_TASK.InitSetTimeTask(TaskWriteEEpromTimer, 5, WriteEEpromTimerCallback);
    static uint16_t CallCount = 0;
    CallCount++;
    if (CallCount % 3 == 0) {
        Task_WriteNowRTCTime();
    }
    Task_WriteValveRunTime();
    // 任意一个事件发生则释放信号量
    EventBits_t uxBits = xEventGroupGetBits(xEventGroup_EEprom);
    if ((uxBits & AllxEvent_EEprom) != 0) {
        xSemaphoreGive(WriteEEpromSemaphore);
    }
}
void WriteEEprom(void * pvParameters) {
    MIN_TASK.InitSetTimeTask(TaskWriteEEpromTimer, 5, WriteEEpromTimerCallback);
    // 创建事件组
    xEventGroup_EEprom = xEventGroupCreate();
    if (xEventGroup_EEprom == NULL) {
        Reboot.EC20_ReBoot_In_RTC_By_1Second(&Reboot, 10);
    }
    WriteEEpromSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    init_EEprom_RunFlag();
    // 等待信号量阻塞任务, 转为运行其他任务
    while (xSemaphoreTake(WriteEEpromSemaphore, portMAX_DELAY) == pdTRUE) {
        isRunEEprom();
        if (xEventGroupGetBits(xEventGroup_EEprom) & WriteNowRTCTime) {
            ShowWait(true);
            xEventGroupClearBits(xEventGroup_EEprom, WriteNowRTCTime); // 清除事件标志位
            EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.Time_Data, sizeof(FL_RTC_InitTypeDef));
            EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.UserSet_DegreePart, sizeof(AT24CXX_Manager_NET.UserSet_DegreePart));
            RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
        }
        if (xEventGroupGetBits(xEventGroup_EEprom) & WriteValveRunTime) {
            xEventGroupClearBits(xEventGroup_EEprom, WriteValveRunTime); // 清除事件标志位
            WriteRunTime(AT24CXX_Manager_NET.RunTimeBack_AddrNum);
        }
        if (xEventGroupGetBits(xEventGroup_EEprom) & WriteAT24C0xxData) {
            ShowWait(true);
            xEventGroupClearBits(xEventGroup_EEprom, WriteAT24C0xxData); // 清除事件标志位
            EEprom_AT24CXX_Parameter_Init(true);
            InitSendModeAndTimeTask();  // 指令跟新 计算当天的发送模式和时间任务
            RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
        }
        clearEEpromFlag;
    }
}
void Task_WriteNowRTCTime(void) {
    // 某个事件发生时设置对应位
    xEventGroupSetBits(xEventGroup_EEprom, WriteNowRTCTime);
}
void Task_WriteValveRunTime(void) {
    if (!readDataBit(AT24CXX_Manager_NET.ModeCode, SaveRunTimeForOpenValve)) {
        return; // 未使能;
    }
    // 某个事件发生时设置对应位
    xEventGroupSetBits(xEventGroup_EEprom, WriteValveRunTime);
}
void Task_WriteAT24C0xxData(void) {
    xEventGroupSetBits(xEventGroup_EEprom, WriteAT24C0xxData);
}


