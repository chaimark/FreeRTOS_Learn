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
void ReadEEpromTimerFromISR(void) {
    BaseType_t xHPW_TaskWoken = pdFALSE;
    MIN_TASK.InitSetTimeTask(TaskReadEEprom, HourToMin(1), ReadEEpromTimerFromISR);
    // 发出信号量, 准备继续运行
    xSemaphoreGiveFromISR(ReadEEpromSemaphore, &xHPW_TaskWoken);
    if (xTaskResumeFromISR(ReadEEpromHand) == pdTRUE) {
        xHPW_TaskWoken = pdTRUE;
    }
    portYIELD_FROM_ISR(xHPW_TaskWoken);
}
void ReadEEprom(void * pvParameters) {
    ReadEEpromSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    init_EEprom_RunFlag();
    MIN_TASK.InitSetTimeTask(TaskReadEEprom, HourToMin(1), ReadEEpromTimerFromISR);
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
void WriteEEpromTimerFromISR(void) {
    static uint16_t CallCount = 0;
    BaseType_t xHPW_TaskWoken = pdFALSE;
    if (System_RunData.isPowerModuleUseing == true) {
        portYIELD_FROM_ISR(xHPW_TaskWoken);
        return;
    }
    MIN_TASK.InitSetTimeTask(TaskWriteEEpromTimer, 5, WriteEEpromTimerFromISR);
    CallCount++;
    if (CallCount % 3 == 0) {
        xEventGroupSetBitsFromISR(xEventGroup_EEprom, WriteNowRTCTime, &xHPW_TaskWoken);
    }
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, SaveRunTimeForOpenValve)) {
        xEventGroupSetBitsFromISR(xEventGroup_EEprom, WriteValveRunTime, &xHPW_TaskWoken);
    }
    portYIELD_FROM_ISR(xHPW_TaskWoken);
}

void WriteEEprom(void * pvParameters) {
    // 创建事件组
    xEventGroup_EEprom = xEventGroupCreate();
    if (xEventGroup_EEprom == NULL) {
        Reboot.EC20_ReBoot_In_RTC_By_1Second(&Reboot, 10);
    }
    init_EEprom_RunFlag();
    MIN_TASK.InitSetTimeTask(TaskWriteEEpromTimer, 5, WriteEEpromTimerFromISR);
    while (1) {
        // printf("%s left:%u\n", pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));
        /* 永久阻塞，直到任意一个事件位被置位 */
        EventBits_t uxBits = xEventGroupWaitBits(
            xEventGroup_EEprom,
            AllxEvent_EEprom,
            pdTRUE,     /* 退出前自动清除对应位 */
            pdFALSE,    /* 任意一位即可唤醒 */
            portMAX_DELAY
        );
        if (uxBits & WriteNowRTCTime) {
            RTC_TASK.CloseTask(HomePageRefresh);
            ShowWait(true);
            isRunEEprom();
            EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.Time_Data, sizeof(FL_RTC_InitTypeDef));
            EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.UserSet_DegreePart, sizeof(AT24CXX_Manager_NET.UserSet_DegreePart));
            clearEEpromFlag;
            RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
        }
        if (uxBits & WriteValveRunTime) {
            isRunEEprom();
            WriteRunTime(AT24CXX_Manager_NET.RunTimeBack_AddrNum);
            clearEEpromFlag;
        }
        if (uxBits & WriteAT24C0xxData) {
            RTC_TASK.CloseTask(HomePageRefresh);
            ShowWait(true);
            isRunEEprom();
            EEprom_AT24CXX_Parameter_Init(true);
            clearEEpromFlag;
            InitSendModeAndTimeTask();  // 指令跟新 计算当天的发送模式和时间任务
            RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
        }

        System_RunData.isPowerModuleUseing = false;
    }
}

/*---------- 两个中断安全包装 ----------*/
void StartWriteNowRTCTimeFromISR(BaseType_t * xHPW_TaskWoken) {
    System_RunData.isPowerModuleUseing = true;
    xEventGroupSetBitsFromISR(xEventGroup_EEprom, WriteNowRTCTime, xHPW_TaskWoken);
}

void StartWriteAT24C0xxDataFromISR(BaseType_t * xHPW_TaskWoken) {
    System_RunData.isPowerModuleUseing = true;
    xEventGroupSetBitsFromISR(xEventGroup_EEprom, WriteAT24C0xxData, xHPW_TaskWoken);
}
