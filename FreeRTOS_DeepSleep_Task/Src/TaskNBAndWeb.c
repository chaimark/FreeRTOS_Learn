#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include <string.h>
#include "Define.h"
#include "Display.h"
#include "RTC.h"
#include "TaskNBAndWeb.h"
#include "NB_Mode_Lib.h"
#include "AT24CXXDataLoader.h"
#include "LPUart_0_And_1_Lib.h"
#include "UpData.h"
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EventGroupHandle_t xEventGroup_NB = NULL;
#define SendData        (1 << 0) // 上传事件
#define CloseNB         (1 << 1) // 关闭 NB 事件
#define CheckStr        (1 << 2) // 监控事件
#define AllxEvent_NB    (SendData | CloseNB | CheckStr)
TaskHandle_t NBModeHand = NULL;
SemaphoreHandle_t NBModeSemaphore = NULL;
TimerHandle_t CloseNB_Timer = NULL;
// 获取指令, 并计算什么时候结束 NB
void Online_Timer(TimerHandle_t xTimer) {
    static uint8_t MaxDelayCount = 3;
    static uint8_t getCount = 0;
    // 定时主动获取（xTimer 周期）
    if ((System_RunData.Now_NetDevParameter.isGetDownCmd == true) && (getCount < MaxDelayCount)) {
        System_RunData.Now_NetDevParameter.GetDownCmd();
    }
    if (System_RunData.ReceiveFlag == true) {   // 收到
        System_RunData.ReceiveFlag = false;
        MaxDelayCount = (UpdataData.Sign == 0xB2 ? 15 : 3); // 升级时，要延长到 15s
        getCount = 0;
    } else if (getCount >= MaxDelayCount) {
        MaxDelayCount = 3;
        getCount = 0;
        // 连续 MaxDelayCount 次以上没收到指令 删除定时器
        xTimerStop(xTimer, pdMS_TO_TICKS(5));
        if (xTimerDelete(xTimer, pdMS_TO_TICKS(5)) != pdPASS) {
            xTimerDelete(xTimer, portMAX_DELAY);
        }
        // 结束 NB 任务
        xEventGroupSetBits(xEventGroup_NB, CloseNB);
        xSemaphoreGive(NBModeSemaphore);
    }
    // 小于 3 次没收到
    getCount++;
}
static uint8_t SendCount = 0;  // 发送次数
void CloseNB_SUB(void) {
    static uint8_t AgainSendDataCount = 0;
    AgainSendDataCount = (System_RunData.Now_NetDevParameter.isSendOk ? 0 : AgainSendDataCount);
    if ((System_RunData.Now_NetDevParameter.isSendOk == false) && (AgainSendDataCount == 0)) {
        AgainSendDataCount++;
        Min_15_TASK.InitSetTimeTask(AgainSendData, 1, NULL);
        SendCount = (SendCount == 0 ? 0 : SendCount - 1); // 发送计数器 -1, 补发不占用发送限制
    }
    InitSendModeAndTimeTask();  // 计算当天的发送模式和时间任务
}

#include "NetProt_Module.h"
void StartNB_SUB(void) {
    CloseNB_Timer = xTimerCreate(
        "CloseNB_Time",         // 定时器名称
        pdMS_TO_TICKS(1000),    // 定时周期
        pdTRUE,                 // 自动重载
        (void *)0,              // 定时器 ID
        Online_Timer            // 回调函数
    );
    SendCount++; // 发送次数
    // ************************************ 判断当天属于什么模式 ************************************ //
    char NowSendFlagMode = readNowSendMode(daysFormOneMonthOneDay(GetDateHex(NowYear), GetDateHex(NowMonth), GetDateHex(NowDay)));
    // ************************************ 判断当天属于什么模式 ************************************ //
    if (SendCount >= AT24CXX_Manager_NET.SendManageObj.SendFlagMode[NowSendFlagMode].OneDayMaxSendTime) {
        SendCount = 0;
        RTC_TASK.CloseTask(SendIntervalTask); // 发送次数大于限制 关闭定时任务
    }
    Net_Task(); // 联网并发送数据
    if (System_RunData.Now_NetDevParameter.NowNetOnlineFlag == false) { // 如果没连上网，直接 CloseNB
        // 结束 NB 任务
        xEventGroupSetBits(xEventGroup_NB, CloseNB);
        xSemaphoreGive(NBModeSemaphore);
    }
    // 启动定时器
    if (xTimerStart(CloseNB_Timer, pdMS_TO_TICKS(5)) != pdPASS) {
        xTimerStart(CloseNB_Timer, portMAX_DELAY);
    }
}
void NBModeTask(void * pvParameters) {
    NBModeSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    while (xSemaphoreTake(NBModeSemaphore, portMAX_DELAY) == pdTRUE) {
        if (xEventGroupGetBits(xEventGroup_NB) & SendData) {
            xEventGroupClearBits(xEventGroup_NB, SendData);  // 清除事件标志位
            SetIsEnterLow_Power(false, 2); // 暂时屏蔽低功耗
            RTC_TASK.CloseTask(HomePageRefresh);
            ShowWait(false);
            StartNB_SUB();
        }
        if (xEventGroupGetBits(xEventGroup_NB) & CloseNB) {
            xEventGroupClearBits(xEventGroup_NB, CloseNB);   // 清除事件标志位
            CloseNB_SUB();
            _Module_Start_Flage = 0;
            RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
            SetIsEnterLow_Power(true, 2); // 低功耗
        }
        if (xEventGroupGetBits(xEventGroup_NB) & CheckStr) {
            xEventGroupClearBits(xEventGroup_NB, CheckStr);   // 清除事件标志位
            SpecialDone();
        }
    }
}
void StartNBMode(bool IsFrom_ISR) {
    // 判断软件定时器是否启动，如果启动说明正在执行 NB 任务
    if (xTimerIsTimerActive(CloseNB_Timer) == pdTRUE) {
        return;
    }
    RTC_TASK.CloseTask(SendIntervalTask);
    xEventGroupSetBits(xEventGroup_NB, SendData);
    // 任意一个事件发生则释放信号量
    EventBits_t uxBits = xEventGroupGetBits(xEventGroup_NB);
    if ((uxBits & AllxEvent_NB) != 0) {
        if (!IsFrom_ISR) {  // 非中断发送信号
            xSemaphoreGive(NBModeSemaphore);
            return;
        }
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // 向 TimeTask 发出信号量, 准备继续运行
        xSemaphoreGiveFromISR(NBModeSemaphore, &xHigherPriorityTaskWoken);
        // 恢复 Task 任务
        if (xTaskResumeFromISR(NBModeHand) == pdFALSE) {
            // 如果需要切换上下文
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
void CheckStr_OK(void) {
    xEventGroupSetBits(xEventGroup_NB, CheckStr);
    // 任意一个事件发生则释放信号量
    EventBits_t uxBits = xEventGroupGetBits(xEventGroup_NB);
    if ((uxBits & AllxEvent_NB) != 0) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // 向 TimeTask 发出信号量, 准备继续运行
        xSemaphoreGiveFromISR(NBModeSemaphore, &xHigherPriorityTaskWoken);
        // 恢复 Task 任务
        if (xTaskResumeFromISR(NBModeHand) == pdFALSE) {
            // 如果需要切换上下文
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}


