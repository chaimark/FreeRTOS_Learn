#include "PublicLib_No_One.h"
#include "UpData.h"
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
#include "TaskNBAndWeb.h"
#include "Display.h"

// TCB 踩踏钩子
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName) {
    /* 断点这里看是谁踩了栈 */
    __asm("bkpt 0");
    for (;;);
}
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
// Sleep
void Sleep(void) {
    FL_RCC_RCMF_Disable();               //关闭RCMF
    FL_RMU_PDR_Enable(RMU);              //打开PDR
    FL_RMU_BORPowerDown_Disable(RMU);    //关闭BOR 2uA

    /*使用ADC时ADCMonitor功能以及Vref需同时开始，同时关闭*/
    FL_VREF_Disable(VREF);               //关闭VREF1p2
    FL_SVD_DisableADCMonitor(SVD);       //关闭ADC电源检测
    FL_ADC_Disable(ADC);                 //关闭ADC使能

    FL_PMU_SetLowPowerMode(PMU, FL_PMU_POWER_MODE_SLEEP_OR_DEEPSLEEP);
    __WFI();
}
int main(void) {
    NVIC_SetVectorTable(0, UPDATA_MCU_OFFSET);
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
    ShowWait(false);
    EEprom_AT24CXX_Parameter_Init(false);
    setNetArgumentInit(NULL);
    StartOPenDevMode();
    CheckMeterNum();
    xTaskCreate(WriteEEprom, "WriteEEprom", configMINIMAL_STACK_SIZE * 2, NULL, WriteEEprom_PRIORITY, &WriteEEpromHand); // 5 -- x
    xTaskCreate(ValveCtrlTask, "ValveCtrl", configMINIMAL_STACK_SIZE * 2, NULL, ValveCtrl_PRIORITY, &ValveCtrlHand); // 5 -- x
    xTaskCreate(GpioTask, "GpioTask", configMINIMAL_STACK_SIZE * 2, NULL, GpioTask_PRIORITY, &GpioTaskHand); // 4 -- x
    xTaskCreate(AllReceive, "AllReceive", configMINIMAL_STACK_SIZE * 3, NULL, AllReceive_PRIORITY, &AllReceive_Hand); // 3 -- x
    xTaskCreate(NBModeTask, "NBSendData", configMINIMAL_STACK_SIZE * 3, NULL, NBSend_PRIORITY, &NBModeHand); // 2 -- x
    xTaskCreate(TimeTask, "TestAll", configMINIMAL_STACK_SIZE * 2, NULL, TimeTask_PRIORITY, &TimeTaskHand); // 2 -- x
    xTaskCreate(ReadEEprom, "ReadEEprom", configMINIMAL_STACK_SIZE * 2, NULL, ReadEEprom_PRIORITY, &ReadEEpromHand); // 1 -- x
    vTaskStartScheduler();
    while (1) {
        Sleep();
    }
}


