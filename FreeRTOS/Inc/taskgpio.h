#ifndef __TASKGPIO_H__
#define __TASKGPIO_H__

#include "fm33lc0xx_fl.h"

//#define CPU_UTILIZATION 1

extern void GPIOTask(void *pvParameters);
extern void GPIO_IRQCallBack(void);
extern uint32_t RunTimeTicks;
extern void BaseTime(void);
#endif 
