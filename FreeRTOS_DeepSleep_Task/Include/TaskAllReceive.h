#ifndef _TASK_ALL_RECEIVE_H
#define _TASK_ALL_RECEIVE_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "SI522A_interface.h"
#include "SI522A_App.h"
#include "RF_CMT2300A_Tx.h"
#include "RF_CMT2300A_Receive.h"

extern TaskHandle_t AllReceive_Hand;
#define AllReceive_PRIORITY (tskIDLE_PRIORITY + 1)
extern void LPUart0CheckBuff(void);
extern void AllReceive(void * pvParameters);
extern void OpenBSTime(void);
extern void RF_ReciveData(void);
extern void OpenUartOfLightOrM1(void);

#endif

