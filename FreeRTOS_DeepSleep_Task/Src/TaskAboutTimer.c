#include "TaskAboutTimer.h"
#include "RTC_SetTime.h"
#include "AT24CXXDataLoader.h"
#include "StrLib.h"
#include "ADC.h"
#include "timers.h"
#include "SX1276.h"

/* FlagBit_Task == 0000 0000
    最后三位代表对应定时任务
    1：0001 == 测电压
    2：0010 == 测温度或压力
    4：0100 == 发送 Loar 数据
*/
extern void Send_RF_DATA(unsigned char FA_ACTIVE);
void SendDataByRF(void) {
    Send_RF_DATA(0x0A);
    return;
}
uint8_t FlagBit_Task = 3;
TaskHandle_t TimeTaskHand;
SemaphoreHandle_t TimeTaskSemaphore = NULL;
void startTimeTask(void) {
    if (RTC_TASK.Task[TestArgument].TimeTask_Falge == true) {
        RTC_TASK.InitSetTimeTask(TestArgument, AT24CXX_Manager_NET.Test_TemperOrPress_Interval, startTimeTask); //复位硬件定时器, 测温度或压力
        FlagBit_Task |= 0x02; // 设置标志位, 测温度或压力
    }
    if (RTC_TASK.Task[SendLoarTask].TimeTask_Falge == true) {
        RTC_TASK.InitSetTimeTask(SendLoarTask, AT24CXX_Manager_NET.SendInterval, startTimeTask);    //复位硬件定时器, 发送数据
        FlagBit_Task |= 0x04; // 设置标志位, 发送数据
    }
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // 向 TimeTask 发出信号量, 准备继续运行
    xSemaphoreGiveFromISR(TimeTaskSemaphore, &xHigherPriorityTaskWoken);
    // 恢复 TimeTask 任务
    if (xTaskResumeFromISR(TimeTaskHand) == pdFALSE) {
        // 如果需要切换上下文
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
void VoltgeTestTimer(TimerHandle_t xTimer) {
    FlagBit_Task |= 0x01;   // 设置标志位, 测电压
    if (TimeTaskSemaphore != NULL) { // 唤醒
        xSemaphoreGive(TimeTaskSemaphore); // 释放信号量
    }
}
void TimeTask(void * pvParameters) {
    TimerHandle_t TestBatVoltgeTimer = xTimerCreate(
        "Measure_Time",          // 定时器名称
        pdMS_TO_TICKS(60000),    // 定时周期 1min
        pdTRUE,                  // 自动重载
        (void *)0,               // 定时器 ID
        VoltgeTestTimer          // 回调函数
    );
    // 创建软件定时器
    if (TestBatVoltgeTimer != NULL) {
        xTimerStart(TestBatVoltgeTimer, 0);  // 启动定时器 测电压
    }
    // 创建硬件定时器
    RTC_TASK.InitSetTimeTask(TestArgument, AT24CXX_Manager_NET.Test_TemperOrPress_Interval, startTimeTask); // 测温度或压力
    // 创建硬件定时器
    RTC_TASK.InitSetTimeTask(SendLoarTask, AT24CXX_Manager_NET.SendInterval, startTimeTask); // 发送数据
    TimeTaskSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    static float Front_TemperOrPress = 0;
    do {
        if (FlagBit_Task & 0x01) {
            FlagBit_Task &= ~0x01;
            // 低字节第一位是1的情况
            Test_BatVoltge();
        }
        if (FlagBit_Task & 0x02) {
            FlagBit_Task &= ~0x02;
            // 低字节第二位是1的情况
            if (AT24CXX_Manager_NET.Meter_Type == 1) {  // 1：PT1000
                TEST_Temperature();
                if (Now_TemperOrPress < 25.0) {
                    continue;   // 低于 25 ° 不需要该机制
                }
                if (!(Front_TemperOrPress - 1 < Now_TemperOrPress && Now_TemperOrPress < Front_TemperOrPress + 1)) {
                    Alarm_Falge = true;
                    vTaskDelay(5000);
                    if (!(Front_TemperOrPress - 1 < Now_TemperOrPress && Now_TemperOrPress < Front_TemperOrPress + 1)) {
                        FlagBit_Task |= 0x04; // 设置标志位, 发送数据
                    }
                } else {
                    Alarm_Falge = false;
                }
            } else if (AT24CXX_Manager_NET.Meter_Type == 2) {  // 2:压力
                TEST_Press();
                if (Now_TemperOrPress < 0.1) {
                    continue;   // 低于 0.1 MPa 不需要该机制
                }
                if (!(Front_TemperOrPress - 0.05 < Now_TemperOrPress && Now_TemperOrPress < Front_TemperOrPress + 0.05)) {
                    Alarm_Falge = true;
                    vTaskDelay(5000);
                    if (!(Front_TemperOrPress - 0.05 < Now_TemperOrPress && Now_TemperOrPress < Front_TemperOrPress + 0.05)) {
                        FlagBit_Task |= 0x04; // 设置标志位, 发送数据
                    }
                } else {
                    Alarm_Falge = false;
                }
            }
        }
        Front_TemperOrPress = Now_TemperOrPress;
        if (FlagBit_Task & 0x04) {
            FlagBit_Task &= ~0x04;
            // 低字节第三位是1的情况
            if (Now_NetDevParameter.NowRunTag != 0) {
                SendDataByRF();
            }
        }
        FlagBit_Task &= 0x0F;
    } while (xSemaphoreTake(TimeTaskSemaphore, portMAX_DELAY) == pdTRUE);   // 等待信号量阻塞任务, 转为运行其他任务
}

