#ifndef SENDDATA_TASK_H
#define SENDDATA_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define SendData_PRIORITY_02    (tskIDLE_PRIORITY + 2)
extern void SendDataForLoar(void *pvParameters);

#endif

