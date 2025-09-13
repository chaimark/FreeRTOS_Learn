#ifndef _TASK_ALL_RECEIVE_H
#define _TASK_ALL_RECEIVE_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "CI522_interface.h"
#include "CI522_App.h"

extern TaskHandle_t AllReceive_Hand;
#define AllReceive_PRIORITY (tskIDLE_PRIORITY + 3)
extern void LPUart0CheckBuff(void);
// extern void Uart1CheckBuff(void);
extern void NBUartCheckBuff(void);
extern void AllReceive(void * pvParameters);
extern void OpenBSTime(bool IsOpenBSTime);
extern void OpenUartOfLightOrM1(void);

#endif

