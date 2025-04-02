#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "Tickless_Hook.h" 
#include "lptim32_timing.h"
#include "semphr.h"
#include "gpio.h"
#include "taskgpio.h"
/**
  ****************************************************************************************************
  * @file    main.c
  * @author  FMSH Application Team
  * @brief   Header file of  LL Module
  ****************************************************************************************************
  * @attention
  *
  * Copyright (c) [2019] [Fudan Microelectronics]
  * THIS SOFTWARE is licensed under the Mulan PSL v1.
  * can use this software according to the terms and conditions of the Mulan PSL v1.
  * You may obtain a copy of Mulan PSL v1 at:
  * http://license.coscl.org.cn/MulanPSL
  * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
  * PURPOSE.
  * See the Mulan PSL v1 for more details.
  *
  ****************************************************************************************************
  */

SemaphoreHandle_t   gpioSemaphore = NULL; 
#define TASK_GPIO_PRIORITY          (tskIDLE_PRIORITY + 2)



int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    
    /* SHOULD BE KEPT!!! */
    MF_Clock_Init();
    
    /* Configure the system clock */
    /* SHOULD BE KEPT!!! */
    MF_SystemClock_Config();
    
    /* Initialize FL Driver Library */
    /* SHOULD BE KEPT!!! */
    FL_Init();


    /* Initialize all configured peripherals */
    /* SHOULD BE KEPT!!! */

    MF_Config_Init();

    GPIO_interrupt_init();  
    LPTIM32_Init();
    
    gpioSemaphore = xSemaphoreCreateBinary();       
    xTaskCreate(GPIOTask, "GPIO", configMINIMAL_STACK_SIZE, NULL, TASK_GPIO_PRIORITY, NULL);      
    vTaskStartScheduler(); 
    while(1)
    {     

    }

}


