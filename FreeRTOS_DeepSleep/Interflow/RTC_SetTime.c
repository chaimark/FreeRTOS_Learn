#include "RTC_SetTime.h"

static void _RTCCloseTask(int TaskAddr);
static void _InitRTCTask(int TaskAddr, uint64_t SetMaxSecNum, void (*TaskFunc)(void));

USER_SET_TASK RTC_TASK = {
    .Task = {0},
    .InitSetTimeTask = _InitRTCTask,
    .CloseTask = _RTCCloseTask,
    .NumberOfTimeTask = RTCTimeTaskMAX, // 定时任务数量
};

static void _RTCCloseTask(int TaskAddr) {
    if ((TaskAddr < 0) || (TaskAddr >= RTCTimeTaskMAX)) {
        return;
    }
    RTC_TASK.Task[TaskAddr].isTaskStart = false; // 初始化标记
    RTC_TASK.Task[TaskAddr].TimeTask_Falge = false;
    RTC_TASK.Task[TaskAddr].CountNumOnceSec = 0; // 复位初始
    RTC_TASK.Task[TaskAddr].TaskFunc = NULL;
}
static void _InitRTCTask(int TaskAddr, uint64_t SetMaxSecNum, void (*TaskFunc)(void)) {
    if ((TaskAddr < 0) || (TaskAddr >= RTCTimeTaskMAX)) {
        return;
    }
    RTC_TASK.Task[TaskAddr].TimeTask_Falge = false; // 初始化标记
    RTC_TASK.Task[TaskAddr].isTaskStart = true;     // 开启
    RTC_TASK.Task[TaskAddr].MaxSecNum = SetMaxSecNum; // 定时任务点
    RTC_TASK.Task[TaskAddr].CountNumOnceSec = 0; // 复位初始
    RTC_TASK.Task[TaskAddr].TaskFunc = TaskFunc;
}
// 计数函数
void CountRTCTask(void) {
    for (int TaskAddr = 0; TaskAddr < RTCTimeTaskMAX; TaskAddr++) {
        if (RTC_TASK.Task[TaskAddr].isTaskStart == false) {
            continue;
        }
        if (RTC_TASK.Task[TaskAddr].CountNumOnceSec < RTC_TASK.Task[TaskAddr].MaxSecNum) {
            RTC_TASK.Task[TaskAddr].CountNumOnceSec++;
        } else {
            RTC_TASK.Task[TaskAddr].TimeTask_Falge = true;
        }
        if (RTC_TASK.Task[TaskAddr].TimeTask_Falge == true) {
            if (RTC_TASK.Task[TaskAddr].TaskFunc != NULL) {
                RTC_TASK.Task[TaskAddr].TaskFunc(); // 注意:该函数, 执行时不要太长, 也不要启动同一个定时器的其他任务
            }
        }
    }
#ifdef TimeSpeedNum
    TimeSpeed();
#endif
}
