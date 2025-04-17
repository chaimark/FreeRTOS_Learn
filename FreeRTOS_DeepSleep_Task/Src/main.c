#include "FreeRTOS.h"
#include "task.h"
#include "Tickless_Hook.h" 
#include "semphr.h"
#include "main.h"
#include "Define.h"
#include "ADC.h"
#include "AT24CXXDataLoader.h"
#include "NumberBaseLib.h"
#include "RTC_SetTime.h"
#include "PublicLib_No_One.h"
#include "TaskLPUart_0_And_1.h"
#include "TaskAboutTimer.h"
#include "TaskReadEEprom.h"

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

int main(void) {
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    MF_GPIO_Init();
    /* SHOULD BE KEPT!!! */
    MF_Clock_Init();
    /* Initialize FL Driver Library */
    /* SHOULD BE KEPT!!! */
    FL_Init();
    /* Initialize all configured peripherals */
    /* SHOULD BE KEPT!!! */
    AT24CXXLoader_Init();	// 初始化参数
    MF_Config_Init();
    EEprom_AT24CXX_Parameter_Init(false);
    StartOPenDevMode();
    CheckMeterNum();
    setNetArgumentInit(NULL);
    xTaskCreate(LPUart_0_And_1_Receive, "AllCmd", configMINIMAL_STACK_SIZE, NULL, LPUart_0_And_1_PRIORITY, &LPUart_0_And_1_Hand); // 1 -- x
    xTaskCreate(ReadEEprom, "ReadEEprom", configMINIMAL_STACK_SIZE, NULL, ReadEEprom_PRIORITY, &ReadEEpromHand); // 4
    xTaskCreate(TimeTask, "TestAll", configMINIMAL_STACK_SIZE, NULL, TimeTask_PRIORITY, &TimeTaskHand); // 2
    vTaskStartScheduler();
    while (1) { // 无任务运行，RTOS 自动进入睡眠
#warning "Now_NetDevParameter.RSSI 怎么获取";
    }
}


