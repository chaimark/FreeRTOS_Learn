#ifndef __MIN_SetTime_H
#define __MIN_SetTime_H
#include "StrLib.h"
#include <stdbool.h>
#include <stdint.h>
#include "PublicLib_No_One.h"
// 将小时转换为分钟
#define HourToMin(x) (x*60)
// 将分钟转换为秒
#define MinToSec(x) (x*60)
// 将小时转换为 15 分钟
#define HourToMin_15(x) (x*4)
// 将小时转换为 10 分钟
#define HourToMin_10(x) (x*6)

// 任务数量
#define _MINTimeTaskMAX 3
// #define Min_10_TimeTaskMAX 0
#define Min_15_TimeTaskMAX 2

#ifdef _MINTimeTaskMAX
#ifdef OPEN_AT_CMD_DEBUG
typedef enum _MINTaskName {
    ATDebug,
    CheckPWMOfClock,
}MINTaskName;
#else
typedef enum _MINTaskName {
    CheckPWMOfClock,
    TaskReadEEprom,
    TaskWriteEEpromTimer,
}MINTaskName;
#endif

#ifdef OPEN_AT_CMD_DEBUG
#define MINTimeTaskMAX (_MINTimeTaskMAX + 2)
#else
#define MINTimeTaskMAX _MINTimeTaskMAX
#endif

typedef struct _USER_MIN_SET_TASK {
    struct {
        bool TimeTask_Falge;    // 当前计时任务是否完成
        bool isTaskStart;       // 当前计时任务是否开启
        uint64_t CountNum;
        uint64_t CountMaxNum;
        void (*TaskFunc)(void);
    } Task[MINTimeTaskMAX];
    void (*InitSetTimeTask)(MINTaskName TaskAddr, uint64_t SetCountMaxNum, void (*TaskFunc)(void));
    void (*CloseTask)(MINTaskName TaskAddr);
    int NumberOfTimeTask;
}USER_MIN_SET_TASK;
extern USER_MIN_SET_TASK MIN_TASK;
extern void MinCountRTCTask(void);
#endif

//////////////////////////////////////////////////////////////
#ifdef Min_10_TimeTaskMAX
typedef enum _Min_10_TaskName {
}Min_10_TaskName;
typedef struct _USER_Min_10_SET_TASK {
    struct {
        bool TimeTask_Falge;    // 当前计时任务是否完成
        bool isTaskStart;       // 当前计时任务是否开启
        uint64_t CountNum;
        uint64_t CountMaxNum;
        void (*TaskFunc)(void);
    } Task[Min_10_TimeTaskMAX];
    void (*InitSetTimeTask)(Min_10_TaskName TaskAddr, uint64_t SetCountMaxNum, void (*TaskFunc)(void));
    void (*CloseTask)(Min_10_TaskName TaskAddr);
    int NumberOfTimeTask;
}USER_Min_10_SET_TASK;
extern USER_Min_10_SET_TASK Min_10_TASK;
extern void Min_10_CountRTCTask(void);
#endif
//////////////////////////////////////////////////////////////
#ifdef Min_15_TimeTaskMAX
typedef enum _Min_15_TaskName {
    AgainSendData,
    DayOverCclkTask,
}Min_15_TaskName;
typedef struct _USER_Min_15_SET_TASK {
    struct {
        bool TimeTask_Falge;    // 当前计时任务是否完成
        bool isTaskStart;       // 当前计时任务是否开启
        uint64_t CountNum;
        uint64_t CountMaxNum;
        void (*TaskFunc)(void);
    } Task[Min_15_TimeTaskMAX];
    void (*InitSetTimeTask)(Min_15_TaskName TaskAddr, uint64_t SetCountMaxNum, void (*TaskFunc)(void));
    void (*CloseTask)(Min_15_TaskName TaskAddr);
    int NumberOfTimeTask;
}USER_Min_15_SET_TASK;
extern USER_Min_15_SET_TASK Min_15_TASK;
extern void Min_15_CountRTCTask(void);
#endif
//////////////////////////////////////////////////////////////
#endif
