#ifndef __RTC_SETTIME_H
#define __RTC_SETTIME_H
#include "StrLib.h"
#include <stdbool.h>
#include <stdint.h>
#include "MIN_SetTime.h"
#include "PublicLib_No_One.h"

// 任务数量
#define _RTCTimeTaskMAX 2

#ifdef OPEN_LOWPWER_DEBUG
typedef enum _RTCTaskName {
    LowPwerDebug,
    IWDTClS,
    SendLoarTask,           // 用于判断什么时候发送数据
}RTCTaskName;
#else
typedef enum _RTCTaskName {
    IWDTClS,
    SendLoarTask,           // 用于判断什么时候发送数据
}RTCTaskName;
#endif

#ifdef OPEN_AT_CMD_DEBUG
#define RTCTimeTaskMAX (_RTCTimeTaskMAX + 1)
#else
#define RTCTimeTaskMAX _RTCTimeTaskMAX
#endif

typedef struct _USER_SET_TASK {
    struct {
        bool TimeTask_Falge;    // 当前计时任务是否完成
        bool isTaskStart;       // 当前计时任务是否开启
        uint64_t CountNum;
        uint64_t CountMaxNum;
        void (*TaskFunc)(void);
    } Task[RTCTimeTaskMAX];
    void (*InitSetTimeTask)(int TaskAddr, uint64_t SetCountMaxNum, void (*TaskFunc)(void));
    void (*CloseTask)(int TaskAddr);
    int NumberOfTimeTask;
}USER_SET_TASK;
extern USER_SET_TASK RTC_TASK;
extern void SecondCountRTCTask(void);
#endif

