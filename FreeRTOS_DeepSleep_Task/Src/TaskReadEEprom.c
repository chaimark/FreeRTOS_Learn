#include "TaskReadEEprom.h"
#include "RTC_SetTime.h"
#include "PublicLib_No_One.h"
#include "AT24CXXDataLoader.h"
#include "StrLib.h"
#include "timers.h"

TaskHandle_t ReadEEpromHand = NULL;
SemaphoreHandle_t ReadEEpromSemaphore = NULL;
void startReadEEprom(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // 向 ReadEEprom 发出信号量, 准备继续运行
    xSemaphoreGiveFromISR(ReadEEpromSemaphore, &xHigherPriorityTaskWoken);
    // 恢复 ReadEEpromHand 任务
    if (xTaskResumeFromISR(ReadEEpromHand) == pdFALSE) {
        // 如果需要切换上下文
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
void ReadEEpromTimerCallback(TimerHandle_t xTimer) {
    static uint16_t CallCount = 0;
    CallCount++;
    if (CallCount % HourToMin(1) == 0) {
        startReadEEprom();
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
    while (1) {
        // 等待信号量阻塞任务, 转为运行其他任务
        if (xSemaphoreTake(ReadEEpromSemaphore, portMAX_DELAY) == pdTRUE) {
            EEprom_AT24CXX_Parameter_Init(false);
        }
    }
}





