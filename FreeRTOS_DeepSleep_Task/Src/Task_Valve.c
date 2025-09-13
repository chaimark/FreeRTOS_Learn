#include "Task_Valve.h"
#include "PublicLib_No_One.h"
#include "AT24CXXDataLoader.h"
#include "StrLib.h"
#include "timers.h"
#include "event_groups.h"
#include "LPUart_0_And_1_Lib.h"

//////////////////////////////////////////////////////////////
SemaphoreHandle_t Valve_RunFlag = NULL;
#define clearValveRunFlag xSemaphoreGive(Valve_RunFlag);\
IncludeDelayMs(1)
bool isRunningForValve(uint16_t MaxDelay_100Tick) {
    uint16_t NowCount = 0;
    // 获取互斥量，防止读写冲突
    while (!(xSemaphoreTake(Valve_RunFlag, 100) == pdTRUE)) {
        NowCount++;
        vTaskDelay(1);
        if ((MaxDelay_100Tick != 0xFF) && (MaxDelay_100Tick * 100 <= NowCount)) {
            return false; // 超时退出
        }
    }
    return true;
}
void init_Valve_RunFlag(void) {
    static bool isInit_RunFlag = false;
    if (isInit_RunFlag) {
        return;
    }
    Valve_RunFlag = xSemaphoreCreateBinary(); // 初始化信号量
    xSemaphoreGive(Valve_RunFlag);
}
//////////////////////////////////////////////////////////////
EventGroupHandle_t xEventGroup_vlave = NULL;
#define OtherValve      (1 << 0)
#define NBCtrlValve     (2 << 0)
#define AllxEvent_Valve (OtherValve | NBCtrlValve)
TaskHandle_t ValveCtrlHand = NULL;
SetValve SetData_Input;
// 供其他任务调用的阀门驱动 api
bool ValveCtrlStart(SetValve SetData) {
    if (isRunningForValve(SetData.MaxDelay_1000Tick * 10) == false) {
        return false;
    }
    if (System_RunData.isPowerModuleUseing == true) {
        return false;
    }
    IncludeDelayMs(1); // 等待 100ms, 确保信号量被释放
    SetData_Input = SetData; // 缓存调用方的设置值
    xEventGroupSetBits(xEventGroup_vlave, OtherValve);
    clearValveRunFlag;
    return true;
}
#include "TaskAboutTimer.h"
SetValve TempSetdata = {
    .DoneId = VALVE_REASON_NB_MQTT_ONENET_RESERVE,
    .MaxDelay_1000Tick = 1,
    .Set_Degree_Part = 0xFFFF,
};
void ValveCtrlTask(void * pvParameters) {
    SetValve Zero = {
        .DoneId = VALVE_REASON_SELF_CHECK,
        .MaxDelay_1000Tick = 0,
        .Set_Degree_Part = 0xFFFF,
    };
    init_Valve_RunFlag();
    // 创建事件组
    xEventGroup_vlave = xEventGroupCreate();
    while (1) {
        // printf("%s left:%u\n", pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));
        /* 永久等待任意事件位 */
        EventBits_t uxBits = xEventGroupWaitBits(
            xEventGroup_vlave,
            AllxEvent_Valve,          // OtherValve | NBCtrlValve
            pdTRUE,                   // 退出时自动清除
            pdFALSE,                  // 任意一位就唤醒
            portMAX_DELAY
        );
        // 等待信号量阻塞任务, 转为运行其他任务
        if (uxBits & NBCtrlValve) {
            if (TempSetdata.Set_Degree_Part == 0xEEEE) {
                StartValveAutoTest();
                TempSetdata.Set_Degree_Part = 0xFFFF;
            } else if (TempSetdata.Set_Degree_Part != 0xFFFF) {
                SetData_Input = TempSetdata;
                xEventGroupSetBits(xEventGroup_vlave, OtherValve);
                System_RunData.CtrlDev_Set_Degree_Part = 0xFFFF;
            }
        }
        if (uxBits & OtherValve) {
            isRunningForValve(0xFF);
            Valve_SetDegree(SetData_Input.Set_Degree_Part, SetData_Input.DoneId);
            SetData_Input = Zero; // 复位设置值.
            clearValveRunFlag;
        }
        System_RunData.isPowerModuleUseing = false;
    }
}

void StartValveRunFromISR(BaseType_t * xHPW_TaskWoken) {
    if (System_RunData.Now_NetDevParameter.isWriteEEprom != false) {
        return;
    }
    if (System_RunData.CtrlDev_Set_Degree_Part == 0xFFFF) {
        return;
    }
    System_RunData.isPowerModuleUseing = true;
    TempSetdata.Set_Degree_Part = System_RunData.CtrlDev_Set_Degree_Part;
    System_RunData.CtrlDev_Set_Degree_Part = 0xFFFF;
    // 设置任务组
    xEventGroupSetBitsFromISR(xEventGroup_vlave, NBCtrlValve, xHPW_TaskWoken);
}
