#ifndef __TASKGPIO_H__
#define __TASKGPIO_H__

#include "fm33lc0xx_fl.h"

extern void GPIOTask(void *pvParameters);
extern void GPIO_IRQCallBack(void);
#endif 
