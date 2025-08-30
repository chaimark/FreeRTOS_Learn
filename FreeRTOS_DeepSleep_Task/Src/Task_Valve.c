#include "Task_Valve.h"
#include "PublicLib_No_One.h"
#include "AT24CXXDataLoader.h"
#include "StrLib.h"
#include "timers.h"
#include "event_groups.h"

//////////////////////////////////////////////////////////////
SemaphoreHandle_t Valve_RunFlag = NULL;
#define clearValveRunFlag xSemaphoreGive(Valve_RunFlag)
bool isRunningForValve(uint8_t MaxDelay_1000Tick) {
    uint8_t NowCount = 0;
    // 获取互斥量，防止读写冲突
    while (!(xSemaphoreTake(Valve_RunFlag, 100) == pdTRUE)) {
        NowCount++;
        vTaskDelay(1000);
        if ((MaxDelay_1000Tick != 0xFF) && (MaxDelay_1000Tick <= NowCount)) {
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
TaskHandle_t ValveCtrlHand = NULL;
SemaphoreHandle_t ValveCtrlSemaphore = NULL;
SetValve SetData_Input;
// 供其他任务调用的阀门驱动 api
bool ValveCtrlStart(SetValve SetData) {
    if (isRunningForValve(SetData.MaxDelay_1000Tick) == false) {
        return false;
    }
    xSemaphoreGive(ValveCtrlSemaphore);
    clearValveRunFlag;
    SetData_Input = SetData; // 缓存调用方的设置值
    return true;
}

void ValveCtrlTask(void * pvParameters) {
    SetValve Zero = {
        .DoneId = VALVE_REASON_RETURN_WATER_TEMP,
        .MaxDelay_1000Tick = 0,
        .Set_Degree_Part = 0,
    };
    ValveCtrlSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    init_Valve_RunFlag();
    while (1) {
        SetData_Input = Zero; // 复位设置值
        // 等待信号量阻塞任务, 转为运行其他任务
        if (xSemaphoreTake(ValveCtrlSemaphore, portMAX_DELAY) == pdTRUE) {
            isRunningForValve(0xFF);
            Valve_SetDegree(SetData_Input.Set_Degree_Part, SetData_Input.DoneId);
            clearValveRunFlag;
        }
    }
}
