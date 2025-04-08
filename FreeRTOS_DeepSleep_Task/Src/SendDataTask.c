#include "SendDataTask.h"
#include "RTC_SetTime.h"
#include "AT24CXXDataLoader.h"
#include "StrLib.h"


void SendDataByUart(void) {
    strnew DataBuff = NEW_NAME(LPUART0Ddata.TxBuf);
    return;
}

void startSendDataForLoar(void) {
    // 向 SendDataTask 发出信号量,准备继续运行

}
void SendDataForLoar(void * pvParameters) {
    // 创建硬件定时器
    RTC_TASK.InitSetTimeTask(SendLoarTask, MinToSec(AT24CXX_Manager_NET.SendInterval), startSendDataForLoar); // 600s 发送数据
    while (1) {
        if (RTC_TASK.Task[SendLoarTask].TimeTask_Falge) { // 需要发送数据
            SendDataByUart();
            RTC_TASK.InitSetTimeTask(SendLoarTask, MinToSec(AT24CXX_Manager_NET.SendInterval), startSendDataForLoar); // 600s 发送数据
        }
        // 等待信号量阻塞任务, 转为运行其他任务

    }
}

