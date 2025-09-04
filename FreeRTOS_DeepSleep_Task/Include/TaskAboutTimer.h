#ifndef TASK_ABOUT_TIMER_H
#define TASK_ABOUT_TIMER_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern TaskHandle_t TimeTaskHand;
#define TimeTask_PRIORITY (tskIDLE_PRIORITY + 2)
extern void TimeTask(void * pvParameters);
extern void StartValveAutoTest(void);
extern void StartLimitCheckTask(void);
// 设置 LED 点亮或关闭，以及对应状态的持续时间
extern void setLED_Or_CtrlLinght(bool CloseLEDStart, bool OpenLEDStart, bool isSetLED, int8_t light_S);
#endif

