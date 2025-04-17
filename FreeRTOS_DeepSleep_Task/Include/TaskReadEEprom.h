#ifndef _TASK_READ_EEPROM_H
#define _TASK_READ_EEPROM_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern TaskHandle_t ReadEEpromHand;
#define ReadEEprom_PRIORITY (tskIDLE_PRIORITY + 4)
extern void ReadEEprom(void *pvParameters);

#endif

