#include "SendData_Task.h"
#include "RTC_SetTime.h"
#include "AT24CXXDataLoader.h"

void SendData_ForLoar(void * pvParameters) {
    // 创建硬件定时器
    RTC_TASK.InitSetTimeTask(SendLoarTask, MinToSec(AT24CXX_Manager_NET.SendInterval), NULL); // 600s 发送数据
    while (1) {
        if (RTC_TASK.Task[SendLoarTask].TimeTask_Falge) { // 需要发送数据
            // Send Data ...
            RTC_TASK.InitSetTimeTask(SendLoarTask, MinToSec(AT24CXX_Manager_NET.SendInterval), NULL); // 600s 发送数据
        }
    }
}

