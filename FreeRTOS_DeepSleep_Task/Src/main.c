#include "PublicLib_No_One.h"
#include "Define.h"
#include "ADC.h"
#include "AT24CXXDataLoader.h"
#include "NumberBaseLib.h"
#include "RTC_SetTime.h"
#include "LPUart_0_And_1_Lib.h"
#include "TaskAboutTimer.h"
#include "TaskAllReceive.h"
#include "TaskEEprom.h"
#include "Task_Valve.h"
#include "TaskGpioInit.h"
#include "Display.h"

// 空闲钩子函数, 任务执行一段时间后必须返回空闲任务
void vApplicationIdleHook(void) {
    RTC_TASK.InitSetTimeTask(IWDTClS, MinToSec(8), NULL); // 8min 内定时器喂狗
}
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
    NVIC_SetVectorTable(0, 0x4000);
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
    ShowWait();
    EEprom_AT24CXX_Parameter_Init(false);
    StartOPenDevMode();
    CheckMeterNum();
    setNetArgumentInit(NULL);
    // // 关闭编程口
    // FL_GPIO_InitTypeDef GPIO_InitStruct;
    // /* PD7 SWD_SWCLK */
    // GPIO_InitStruct.pin = FL_GPIO_PIN_7;
    // GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    // GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    // GPIO_InitStruct.pull = FL_DISABLE;
    // GPIO_InitStruct.remapPin = FL_DISABLE;
    // FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    // /* PD8 SWD_SWDIO */
    // GPIO_InitStruct.pin = FL_GPIO_PIN_8;
    // GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    // GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    // GPIO_InitStruct.pull = FL_DISABLE;
    // GPIO_InitStruct.remapPin = FL_DISABLE;
    // FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    xTaskCreate(ReadEEprom, "ReadEEprom", configMINIMAL_STACK_SIZE, NULL, ReadEEprom_PRIORITY, &ReadEEpromHand); // 4
    xTaskCreate(WriteEEprom, "WriteEEprom", configMINIMAL_STACK_SIZE, NULL, WriteEEprom_PRIORITY, &ReadEEpromHand); // 4
    xTaskCreate(GpioTask, "GpioTask", configMINIMAL_STACK_SIZE, NULL, GpioTask_PRIORITY, &GpioTaskHand); // 2
    xTaskCreate(TimeTask, "TestAll", configMINIMAL_STACK_SIZE, NULL, TimeTask_PRIORITY, &TimeTaskHand); // 2
    xTaskCreate(AllReceive, "AllReceive", configMINIMAL_STACK_SIZE, NULL, AllReceive_PRIORITY, &AllReceive_Hand); // 1 -- x
    xTaskCreate(ValveCtrlTask, "ValveCtrl", configMINIMAL_STACK_SIZE, NULL, ValveCtrl_PRIORITY, &ValveCtrlHand); // 1 -- x
    vTaskStartScheduler();
    while (1) { // 无任务运行，RTOS 自动进入睡眠
    }
}


