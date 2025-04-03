#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Tickless_Hook.h" 
#include "semphr.h"
#include "gpio.h"
#include "taskgpio.h"
#include "../Interflow/Define.h"
#include "../Interflow/PublicLib_No_One.h"
#include "../Interflow/AT24CXXDataLoader.h"
#include "../Interflow/NumberBaseLib.h"
#include "../Interflow/RTC_SetTime.h"
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
void AAA_ADD(void * pvParameters) {
    return;
}

int main(void) {
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    /* SHOULD BE KEPT!!! */
    MF_Clock_Init();
    /* Initialize FL Driver Library */
    /* SHOULD BE KEPT!!! */
    FL_Init();
    /* Initialize all configured peripherals */
    /* SHOULD BE KEPT!!! */
    AT24CXXLoader_Init();	// 初始化参数
    MF_Config_Init();
    Device_Init();            // 初始化设备
    EEprom_AT24CXX_Parameter_Init(false);
    // LPTIM32_Init();
    StartOPenDevMode();
    CheckMeterNum();
    TaskHandle_t AAA = NULL;
    gpioSemaphore = xSemaphoreCreateBinary();
    xTaskCreate(AAA_ADD, "AAA", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, AAA);
    vTaskStartScheduler();
    while (1) {

    }

}


