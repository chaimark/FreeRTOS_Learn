#include "ReadEEpromTask.h"
#include "RTC_SetTime.h"
#include "AT24CXXDataLoader.h"
#include "StrLib.h"

void startReadEEprom(void) {
    // 向 MeasureForworld 发出信号量,准备继续运行

}
void ReadEEprom(void * pvParameters) {
    // 创建硬件定时器
    RTC_TASK.InitSetTimeTask(Read_EEprom, MinToSec(1), startReadEEprom); // 600s 发送数据
    while (1) {
        if (RTC_TASK.Task[Read_EEprom].TimeTask_Falge) { // 需要发送数据
            EEprom_AT24CXX_Parameter_Init(false);
            RTC_TASK.InitSetTimeTask(Read_EEprom, MinToSec(1), startReadEEprom); // 600s 发送数据
        }
        // 等待信号量阻塞任务, 转为运行其他任务

    }
}





