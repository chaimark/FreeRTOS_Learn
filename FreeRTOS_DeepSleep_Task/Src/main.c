#include "../Include/main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Tickless_Hook.h" 
#include "semphr.h"
#include "../Include/Define.h"
#include "AT24CXXDataLoader.h"
#include "NumberBaseLib.h"
#include "RTC_SetTime.h"
#include "PublicLib_No_One.h"
#include "SendDataTask.h"
#include "MeasureTask.h"
#include "ReadEEpromTask.h"

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
    StartOPenDevMode();
    CheckMeterNum();
    setNetArgumentInit(NULL);
    TaskHandle_t ReadEEpromHand = NULL;
    TaskHandle_t LoarSendHand = NULL;
    TaskHandle_t MeasureHand = NULL;
    gpioSemaphore = xSemaphoreCreateBinary();
    xTaskCreate(ReadEEprom, "ReadEEprom", configMINIMAL_STACK_SIZE, NULL, ReadEEprom_PRIORITY_01, &ReadEEpromHand);
    xTaskCreate(SendDataForLoar, "LoarSend", configMINIMAL_STACK_SIZE, NULL, SendData_PRIORITY_02, &LoarSendHand);
    xTaskCreate(MeasureForworld, "TestAll", configMINIMAL_STACK_SIZE, NULL, Measure_PRIORITY_02, &MeasureHand);
    vTaskStartScheduler();
    while (1) {

    }
}


