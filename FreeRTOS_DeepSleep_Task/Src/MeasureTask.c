#include "MeasureTask.h"
#include "RTC_SetTime.h"
#include "AT24CXXDataLoader.h"
#include "StrLib.h"
#include "ADC.h"

void MeasureAll(void) {
#ifdef IsTemperature
    Test_InsideT();
#else
    Get_Press_Value();
#endif
    Test_BatVoltge();
}
void startMeasureForworld(void) {
    // 向 MeasureForworld 发出信号量,准备继续运行

}
void MeasureForworld(void * pvParameters) {
    // 创建硬件定时器
    RTC_TASK.InitSetTimeTask(TestArgument, MinToSec(1), startMeasureForworld); // 600s 发送数据
    while (1) {
        if (RTC_TASK.Task[TestArgument].TimeTask_Falge) { // 需要发送数据
            MeasureAll();
            RTC_TASK.InitSetTimeTask(TestArgument, MinToSec(1), startMeasureForworld); // 600s 发送数据
        }
        // 等待信号量阻塞任务, 转为运行其他任务

    }
}
