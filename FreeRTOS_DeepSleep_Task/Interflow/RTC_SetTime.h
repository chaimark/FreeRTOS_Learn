#ifndef __RTC_SETTIME_H
#define __RTC_SETTIME_H
#include "StrLib.h"
#include <stdbool.h>
#include <stdint.h>
#include "PublicLib_No_One.h"
// 将小时转换为分钟
#define HourToMin(x) (x*60)
// 将分钟转换为秒
#define MinToSec(x) (x*60)

#ifdef OPEN_AT_CMD_DEBUG
typedef enum _RTCTaskName {
    ATDebug,
    BatVoltge,          // 用于判断什么时候检测电池电压
    TestTemperOrPressure,            // 用于判断什么时候检测内部温度
    Read_EEprom,        // 用于判断什么时候读取 eeprom
    CheckPWMOfClock,
}RTCTaskName;
#else
typedef enum _RTCTaskName {
    BatVoltge,              // 用于判断什么时候检测电池电压
    TestTemperOrPressure,   // 用于判断当前温度或压力
    Read_EEprom,            // 用于判断什么时候读取 eeprom
    SendLoarTask,           // 用于判断什么时候发送数据
    CheckPWMOfClock,
}RTCTaskName;
#endif

// 任务数量
#define _RTCTimeTaskMAX 5

#ifdef OPEN_AT_CMD_DEBUG
#define RTCTimeTaskMAX (_RTCTimeTaskMAX + 1)
#else
#define RTCTimeTaskMAX _RTCTimeTaskMAX
#endif

typedef struct _USER_SET_TASK {
    struct {
        bool TimeTask_Falge;    // 当前计时任务是否完成
        bool isTaskStart;       // 当前计时任务是否开启
        uint64_t CountNumOnceSec;
        uint64_t MaxSecNum;
        void (*TaskFunc)(void);
    } Task[RTCTimeTaskMAX];
    void (*InitSetTimeTask)(int TaskAddr, uint64_t SetMaxSecNum, void (*TaskFunc)(void));
    void (*CloseTask)(int TaskAddr);
    int NumberOfTimeTask;
}USER_SET_TASK;
extern USER_SET_TASK RTC_TASK;
extern void CountRTCTask(void);
#endif

