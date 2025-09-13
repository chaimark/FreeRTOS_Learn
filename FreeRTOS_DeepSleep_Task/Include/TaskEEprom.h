#ifndef _TASK_READ_EEPROM_H
#define _TASK_READ_EEPROM_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern TaskHandle_t ReadEEpromHand;
#define ReadEEprom_PRIORITY (tskIDLE_PRIORITY + 1)
extern void ReadEEprom(void * pvParameters);

extern TaskHandle_t WriteEEpromHand;
#define WriteEEprom_PRIORITY (tskIDLE_PRIORITY + 5)
extern void WriteEEprom(void * pvParameters);

extern SemaphoreHandle_t EEprom_RunFlag;
extern void isRunEEprom(void);
#define clearEEpromFlag xSemaphoreGive(EEprom_RunFlag);\
IncludeDelayMs(1)

extern void StartWriteNowRTCTimeFromISR(BaseType_t * xHPW_TaskWoken);
extern void StartWriteAT24C0xxDataFromISR(BaseType_t * xHPW_TaskWoken);
#endif

