#ifndef READ_EEPROM_TASK_H
#define READ_EEPROM_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define ReadEEprom_PRIORITY_01    (tskIDLE_PRIORITY + 1)
extern void ReadEEprom(void *pvParameters);

#endif

