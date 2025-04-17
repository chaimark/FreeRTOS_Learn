#ifndef TASK_ABOUT_TIMER_H
#define TASK_ABOUT_TIMER_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern TaskHandle_t TimeTaskHand;
#define TimeTask_PRIORITY (tskIDLE_PRIORITY + 2)
extern void TimeTask(void *pvParameters);

#endif

