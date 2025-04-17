/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : mf_config.c
  * @brief          : MCU FUNCTION CONFIG
  ******************************************************************************
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
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "mf_config.h"
#include "fm33lc0xx_fl.h"
#include "ADC.h"
#include "PublicLib_No_One.h"
/* Private function prototypes -----------------------------------------------*/
void MF_IWDT_Init(void) {
    FL_IWDT_InitTypeDef    IWDT_InitStruct;

    IWDT_InitStruct.overflowPeriod = FL_IWDT_PERIOD_4000MS;
    IWDT_InitStruct.iwdtWindows = 0;

    FL_IWDT_Init(IWDT, &IWDT_InitStruct);
}

void MF_EXTI_Init(void) {
    FL_EXTI_CommonInitTypeDef    EXTI_InitStruct;
    EXTI_InitStruct.clockSource = FL_RCC_EXTI_CLK_SOURCE_LSCLK;
    FL_EXTI_CommonInit(&EXTI_InitStruct);
}
void MF_EXTI_Line_Init(void) {
    FL_GPIO_InitTypeDef     GPIO_InitStruct;
    FL_EXTI_InitTypeDef     EXTI_InitStruct;
    FL_NVIC_ConfigTypeDef   InterruptConfigStruct;
    MF_EXTI_Init();

    // INT1 PB2 EXET4
    GPIO_InitStruct.pin = FL_GPIO_PIN_2;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    EXTI_InitStruct.input = FL_GPIO_EXTI_INPUT_GROUP2;
    EXTI_InitStruct.triggerEdge = FL_GPIO_EXTI_TRIGGER_EDGE_FALLING;
    EXTI_InitStruct.filter = FL_ENABLE;
    FL_EXTI_Init(FL_GPIO_EXTI_LINE_4, &EXTI_InitStruct);
    FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_4);
    /////////////////////////////////// 
    InterruptConfigStruct.preemptPriority = 0x0001;
    FL_NVIC_Init(&InterruptConfigStruct, GPIO_IRQn);
}
void MF_PMU_Init(void) {
    FL_PMU_SleepInitTypeDef    PMU_InitStruct;

    PMU_InitStruct.deepSleep = FL_PMU_SLEEP_MODE_DEEP;
    PMU_InitStruct.LDOLowPowerMode = FL_PMU_LDO_LPM_DISABLE;
    PMU_InitStruct.wakeupFrequency = FL_PMU_RCHF_WAKEUP_FREQ_8MHZ;
    PMU_InitStruct.wakeupDelay = FL_PMU_WAKEUP_DELAY_2US;
    PMU_InitStruct.coreVoltageScaling = FL_DISABLE;

    FL_PMU_Sleep_Init(PMU, &PMU_InitStruct);
}

void MF_RCC_XTLF_IO_Init(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct;
    /* PD9 RCC_XT32KO */
    GPIO_InitStruct.pin = FL_GPIO_PIN_9;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    /* PD10 RCC_XT32KI */
    GPIO_InitStruct.pin = FL_GPIO_PIN_10;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/* Private function prototypes -----------------------------------------------*/
void GPIO_FM33LC02X_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    // PA1_MOSI  PA2_NSS   PA3_SCLK   PA4_RF_Control    PA6_BT_CON   PA7_RST_433    PA9_GP2_RST   PA10_GP2_SI   
    GPIO_InitStruct.pin = FL_GPIO_PIN_1 | FL_GPIO_PIN_2 | FL_GPIO_PIN_3 | FL_GPIO_PIN_4 | FL_GPIO_PIN_6 | FL_GPIO_PIN_7 | FL_GPIO_PIN_9 | FL_GPIO_PIN_10;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_1 | FL_GPIO_PIN_2 | FL_GPIO_PIN_3 | FL_GPIO_PIN_4 | FL_GPIO_PIN_6 | FL_GPIO_PIN_7 | FL_GPIO_PIN_9 | FL_GPIO_PIN_10);
    //FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_9);

    //PA0_MISO  PA8_GP2_SO   PA15_BT_CHECK
    GPIO_InitStruct.pin = FL_GPIO_PIN_0 | FL_GPIO_PIN_8 | FL_GPIO_PIN_15;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    //PB3_GP2_SCK   PB4_GP2_SSN   PB10_GP2_PW_CTR    PB14_PRESS_CTR
    GPIO_InitStruct.pin = FL_GPIO_PIN_3 | FL_GPIO_PIN_4 | FL_GPIO_PIN_10 | FL_GPIO_PIN_14;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_3 | FL_GPIO_PIN_4 | FL_GPIO_PIN_10 | FL_GPIO_PIN_14);

    // PB2_GD0_433   PB5_GP2_INT    //PB12_ACLK 
    GPIO_InitStruct.pin = FL_GPIO_PIN_2 | FL_GPIO_PIN_5;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //PB8   PB9   PB10  PB11  PB12  PB13
    GPIO_InitStruct.pin = FL_GPIO_PIN_8 | FL_GPIO_PIN_9 | FL_GPIO_PIN_10 | FL_GPIO_PIN_11 | FL_GPIO_PIN_12 | FL_GPIO_PIN_13;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    FL_GPIO_SetOutputPin(GPIOB, FL_GPIO_PIN_8 | FL_GPIO_PIN_9 | FL_GPIO_PIN_10 | FL_GPIO_PIN_11 | FL_GPIO_PIN_12 | FL_GPIO_PIN_13);

    //PC7_PRESS_IN     PC9_VCC2
    GPIO_InitStruct.pin = FL_GPIO_PIN_7 | FL_GPIO_PIN_9;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // PC0_SDATA  PC1_SCLK
    GPIO_InitStruct.pin = FL_GPIO_PIN_0 | FL_GPIO_PIN_1;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_0 | FL_GPIO_PIN_1);

    //PC4 PC5 PC6  PC8  PC10
    GPIO_InitStruct.pin = FL_GPIO_PIN_4 | FL_GPIO_PIN_5 | FL_GPIO_PIN_6 | FL_GPIO_PIN_8 | FL_GPIO_PIN_10;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_4 | FL_GPIO_PIN_5 | FL_GPIO_PIN_6 | FL_GPIO_PIN_8 | FL_GPIO_PIN_10);

    //PD0 PD1 PD6 PD11
    GPIO_InitStruct.pin = FL_GPIO_PIN_0 | FL_GPIO_PIN_1 | FL_GPIO_PIN_6 | FL_GPIO_PIN_11;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    FL_GPIO_SetOutputPin(GPIOD, FL_GPIO_PIN_0 | FL_GPIO_PIN_1 | FL_GPIO_PIN_6 | FL_GPIO_PIN_11);
}
/**
  * @brief  GPIO Initialization function
  * @param  void
  * @retval None
  */
void MF_GPIO_Init(void) {
    // 默认设置 开漏输出 1 (设置为高阻态)
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_ALL_EXCEPTSWD;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT; // 输出模式
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN; // 开漏输出
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SET_L(GPIOA, FL_GPIO_PIN_ALL_EXCEPTSWD);
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_SET_L(GPIOB, FL_GPIO_PIN_ALL_EXCEPTSWD);
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_SET_L(GPIOC, FL_GPIO_PIN_ALL_EXCEPTSWD);
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    GPIO_SET_L(GPIOD, FL_GPIO_PIN_ALL_EXCEPTSWD);
    // GPIO_FM33LC02X_Init();
}

/**
  * @brief  The application entry point.
  * @retval int
  */
void MF_Clock_Init(void) {
    /* Initial XTLF */
    MF_RCC_XTLF_IO_Init();
    FL_RCC_XTLF_SetWorkCurrent(FL_RCC_XTLF_WORK_CURRENT_450NA);    /* 设置工作电流为最大以便快速起振 */
    FL_RCC_XTLF_Enable();
    /* Initial Low Speed Clock */
    FL_RCC_SetLSCLKClockSource(FL_RCC_LSCLK_CLK_SOURCE_XTLF);
    // FL_RCC_SetLSCLKClockSource(FL_RCC_LSCLK_CLK_SOURCE_LPOSC);
    /* Initial RCHF */
    FL_RCC_RCHF_SetFrequency(FL_RCC_RCHF_FREQUENCY_8MHZ);
    FL_RCC_RCHF_Enable();
    /* Initialize all configured peripherals */
    FL_RCC_SetSystemClockSource(FL_RCC_SYSTEM_CLK_SOURCE_RCHF);
    FL_RCC_SetAHBPrescaler(FL_RCC_AHBCLK_PSC_DIV1);
    FL_RCC_SetAPB1Prescaler(FL_RCC_APB1CLK_PSC_DIV1);
    FL_RCC_SetAPB2Prescaler(FL_RCC_APB2CLK_PSC_DIV1);
    /* MCU Configuration--------------------------------------------------------*/
    FL_RCC_EnableGroup1BusClock(FL_RCC_GROUP1_BUSCLK_RTC);
    FL_RCC_DisableGroup1BusClock(FL_RCC_GROUP1_BUSCLK_RTC);
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    /* System interrupt init*/
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void MF_SystemClock_Config(void) {

}

#include "GP21.h"
#include "SX1276.h"
void MF_Config_Init(void) {
    /* Initial PMU */
    MF_PMU_Init();
    /* initial GP2 */
    GP21_GPIO_Init();
    /* initial SX1276 */
    SX1276_GPIO_Init();
    /* Initial ADC */
    MF_ADC_Common_Init();
    MF_ADC_Sampling_Init();
    IncludeDelayMs(100);
    Config_Init_XTHF_And_XTLF();
    Device_Init();      // 初始化设备
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t * file, uint32_t line) {
  /* USER CODE BEGIN Assert_Failed */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END Assert_Failed */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT FMSH *****END OF FILE****/
