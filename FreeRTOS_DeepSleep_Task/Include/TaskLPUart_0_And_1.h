#ifndef _TASK_LPUart_0_And_1_H
#define _TASK_LPUart_0_And_1_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern TaskHandle_t LPUart_0_And_1_Hand;
#define LPUart_0_And_1_PRIORITY (tskIDLE_PRIORITY + 1)
extern void LPUart0CheckBuff(void);
extern void LPUart1CheckBuff(void);
extern void LPUart_0_And_1_Receive(void * pvParameters);
extern void OpenBSTime(void);

#endif

