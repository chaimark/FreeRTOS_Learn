#ifndef MEASURE_TASK_H
#define MEASURE_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define Measure_PRIORITY_02    (tskIDLE_PRIORITY + 2)
extern void MeasureForworld(void *pvParameters);

#endif


