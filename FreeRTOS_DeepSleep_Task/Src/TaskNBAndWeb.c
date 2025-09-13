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
#include "_Module_ATCmd_FunctionSum.h"
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EventGroupHandle_t xEventGroup_NB = NULL;
#define SendData_NB        (1 << 0) // 上传事件
#define CloseNB_NB         (1 << 1) // 关闭 NB 事件
#define CheckStr_NB        (1 << 2) // 监控事件
#define AllxEvent_NB    (SendData_NB | CloseNB_NB | CheckStr_NB)
TaskHandle_t NBModeHand = NULL;
TimerHandle_t CloseNB_Timer = NULL;
#define MaxCount 5
// 获取指令, 并计算什么时候结束 NB
void Online_Timer(TimerHandle_t xTimer) {
    static uint8_t MaxDelayCount = MaxCount;
    static uint8_t getCount = 0;
    // 定时主动获取（xTimer 周期）
    if ((System_RunData.Now_NetDevParameter.isGetDownCmd == true) && (getCount < MaxDelayCount)) {
        System_RunData.Now_NetDevParameter.GetDownCmd();
    }
    if (System_RunData.ReceiveFlag == true) {   // 收到
        System_RunData.ReceiveFlag = false;
        MaxDelayCount = ((uint8_t)UpdataData.Sign == 0xB2 ? 15 : MaxCount); // 升级时，要延长到 15s
        getCount = 0;
    } else if (getCount >= MaxDelayCount) {
        MaxDelayCount = MaxCount;
        getCount = 0;
        // 连续 MaxDelayCount 次以上没收到指令 删除定时器
        xTimerStop(xTimer, 0);
        // 结束 NB 任务
        xEventGroupSetBits(xEventGroup_NB, CloseNB_NB);
        return;
    }
    // 小于 MaxCount 次没收到
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
    Shutdown_Module_();
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
        xEventGroupSetBits(xEventGroup_NB, CloseNB_NB);
        return;
    }
    // 启动定时器
    if (xTimerStart(CloseNB_Timer, pdMS_TO_TICKS(5)) != pdPASS) {
        xTimerStart(CloseNB_Timer, portMAX_DELAY);
    }
}
void NBModeTask(void * pvParameters) {
    // 创建事件组
    xEventGroup_NB = xEventGroupCreate();
    while (1) {
        // printf("%s left:%u\n", pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));
        /* 永久等待任意事件位 */
        EventBits_t uxBits = xEventGroupWaitBits(
            xEventGroup_NB,
            AllxEvent_NB,          // OtherValve | NBCtrlValve
            pdTRUE,                   // 退出时自动清除
            pdFALSE,                  // 任意一位就唤醒
            portMAX_DELAY
        );
        if (uxBits & SendData_NB) {
            xEventGroupClearBits(xEventGroup_NB, SendData_NB);  // 清除事件标志位
            SetIsEnterLow_Power(false, 2); // 暂时屏蔽低功耗
            RTC_TASK.CloseTask(HomePageRefresh);
            ShowWait(false);
            StartNB_SUB();
        }
        if (uxBits & CloseNB_NB) {
            xEventGroupClearBits(xEventGroup_NB, CloseNB_NB);   // 清除事件标志位
            CloseNB_SUB();
            if (CloseNB_Timer != NULL) {
                // 确保定时器停止运行
                if (xTimerIsTimerActive(CloseNB_Timer) != pdFALSE) {
                    xTimerStop(CloseNB_Timer, 0);
                    // 删除定时器
                    if (xTimerDelete(CloseNB_Timer, 0) == pdPASS) {
                        CloseNB_Timer = NULL; // 手动将句柄置为 NULL，避免野指针
                    }
                }
            }
            SetIsEnterLow_Power(true, 2); // 低功耗
            RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
            System_RunData.Now_NetDevParameter.NowNetOnlineFlag = false;
            _Module_Start_Flage = 0;
            System_RunData.isPowerModuleUseing = false;
        }
        if (uxBits & CheckStr_NB) {
            xEventGroupClearBits(xEventGroup_NB, CheckStr_NB);   // 清除事件标志位
            SpecialDoneFromTask(System_RunData.Now_NetDevParameter.Special_ID);
        }
    }
}

void StartNBModeFromISR(BaseType_t * xHPW_TaskWoken) {
    if (System_RunData.isPowerModuleUseing == true) {
        return;                 /* 早期返回，仍在中断里，安全 */
    }
    System_RunData.isPowerModuleUseing = true;
    RTC_TASK.CloseTask(SendIntervalTask);
    // 设置任务组
    xEventGroupSetBitsFromISR(xEventGroup_NB, SendData_NB, xHPW_TaskWoken);
}

void CheckStr_OK_FromISR(void) {
    BaseType_t xHPW_TaskWoken = pdFALSE;
    // 设置任务组
    xEventGroupSetBitsFromISR(xEventGroup_NB, CheckStr_NB, &xHPW_TaskWoken);
    portYIELD_FROM_ISR(xHPW_TaskWoken);
}

