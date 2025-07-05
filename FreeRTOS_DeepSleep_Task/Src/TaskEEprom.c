#include "TaskEEprom.h"
#include "PublicLib_No_One.h"
#include "AT24CXXDataLoader.h"
#include "RTC_SetTime.h"
#include "StrLib.h"
#include "timers.h"
#include "event_groups.h"
SemaphoreHandle_t EEprom_RunFlag = NULL;
#define clearEEpromFlag xSemaphoreGive(EEprom_RunFlag)
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
void ReadEEpromTimerCallback(TimerHandle_t xTimer) {
    static uint16_t CallCount = 0;
    CallCount++;
    if (CallCount % HourToMin(1) == 0) {
        xSemaphoreGive(ReadEEpromSemaphore);
        CallCount = 0;
    }
}
void ReadEEprom(void * pvParameters) {
    TimerHandle_t ReadEEpromTimer = xTimerCreate(
        "ReadEEprom_Time",          // 定时器名称
        pdMS_TO_TICKS(60000),       // 定时周期
        pdTRUE,                     // 自动重载
        (void *)0,                  // 定时器 ID
        ReadEEpromTimerCallback     // 回调函数
    );
    if (ReadEEpromTimer != NULL) {
        xTimerStart(ReadEEpromTimer, 0);  // 启动定时器
    }
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
    RunTimeNum += Get_Module_Data.TempRunTimeForValve; // 已运行 Get_Module_Data.TempRunTimeForValve 分钟
    Get_Module_Data.TempRunTimeForValve = 0;
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
#define AllxEvent_EEprom (WriteNowRTCTime | WriteValveRunTime)
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

TaskHandle_t WriteEEpromHand = NULL;
SemaphoreHandle_t WriteEEpromSemaphore = NULL;
void WriteEEpromTimerCallback(TimerHandle_t xTimer) {
    static uint16_t CallCount = 0;
    CallCount++;
    if (CallCount % 15 == 0) {
        Task_WriteNowRTCTime();
    }
    if (CallCount % 5 == 0) {
        Task_WriteValveRunTime();
    }
    // 任意一个事件发生则释放信号量
    EventBits_t uxBits = xEventGroupGetBits(xEventGroup_EEprom);
    if ((uxBits & AllxEvent_EEprom) != 0) {
        xSemaphoreGive(WriteEEpromSemaphore);
    }
}
void WriteEEprom(void * pvParameters) {
    TimerHandle_t WriteEEpromTimer = xTimerCreate(
        "WriteEEprom_Time",         // 定时器名称
        pdMS_TO_TICKS(60000),       // 定时周期
        pdTRUE,                     // 自动重载
        (void *)0,                  // 定时器 ID
        WriteEEpromTimerCallback    // 回调函数
    );
    if (WriteEEpromTimer != NULL) {
        xTimerStart(WriteEEpromTimer, 0);  // 启动定时器
    }
    // 创建事件组
    xEventGroup_EEprom = xEventGroupCreate();
    if (xEventGroup_EEprom == NULL) {
        Reboot.EC20_ReBoot_In_RTC_By_1Second(&Reboot, 10);
    }
    WriteEEpromSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    init_EEprom_RunFlag();
    while (1) {
        // 等待信号量阻塞任务, 转为运行其他任务
        if (xSemaphoreTake(WriteEEpromSemaphore, portMAX_DELAY) == pdTRUE) {
            isRunEEprom();
            if (xEventGroupGetBits(xEventGroup_EEprom) & WriteNowRTCTime) {
                xEventGroupClearBits(xEventGroup_EEprom, WriteNowRTCTime); // 清除事件标志位
                EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.Time_Data, sizeof(FL_RTC_InitTypeDef));
            }
            if (xEventGroupGetBits(xEventGroup_EEprom) & WriteValveRunTime) {
                xEventGroupClearBits(xEventGroup_EEprom, WriteValveRunTime); // 清除事件标志位
                WriteRunTime(AT24CXX_Manager_NET.RunTimeBack_AddrNum);
            }
            clearEEpromFlag;
        }
    }
}



