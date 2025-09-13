#ifndef NB_MODE_AND_WEB_H
#define NB_MODE_AND_WEB_H
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
/////////////////////////////////////////////////
extern TaskHandle_t NBModeHand;
#define NBSend_PRIORITY (tskIDLE_PRIORITY + 2)
extern void NBModeTask(void * pvParameters);
extern void StartNBModeFromISR(BaseType_t * xHPW_TaskWoken);
extern void CheckStr_OK_FromISR(void);
#endif
