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

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  GPIO Initialization function
  * @param  void
  * @retval None
  */
void MF_GPIO_Init(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct;

    FL_GPIO_SetOutputPin(GPIOC, FL_GPIO_PIN_0);

    GPIO_InitStruct.pin = FL_GPIO_PIN_0;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;

    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}


/**
  * @brief  The application entry point.
  * @retval int
  */
void MF_Clock_Init(void) {
    #warning need check;
    // /* Initial XTLF */
    // MF_RCC_XTLF_IO_Init();
    // FL_RCC_XTLF_SetWorkCurrent(FL_RCC_XTLF_WORK_CURRENT_450NA);    /* 设置工作电流为最大以便快速起振 */
    // FL_RCC_XTLF_Enable();
    // /* Initial Low Speed Clock */
    // FL_RCC_SetLSCLKClockSource(FL_RCC_LSCLK_CLK_SOURCE_XTLF);
    // // FL_RCC_SetLSCLKClockSource(FL_RCC_LSCLK_CLK_SOURCE_LPOSC);
    // /* Initial RCHF */
    // FL_RCC_RCHF_SetFrequency(FL_RCC_RCHF_FREQUENCY_8MHZ);
    // FL_RCC_RCHF_Enable();
    // /* Initialize all configured peripherals */
    // FL_RCC_SetSystemClockSource(FL_RCC_SYSTEM_CLK_SOURCE_RCHF);
    // FL_RCC_SetAHBPrescaler(FL_RCC_AHBCLK_PSC_DIV1);
    // FL_RCC_SetAPB1Prescaler(FL_RCC_APB1CLK_PSC_DIV1);
    // FL_RCC_SetAPB2Prescaler(FL_RCC_APB2CLK_PSC_DIV1);
    // /* MCU Configuration--------------------------------------------------------*/
    // FL_RCC_EnableGroup1BusClock(FL_RCC_GROUP1_BUSCLK_RTC);
    // FL_RCC_DisableGroup1BusClock(FL_RCC_GROUP1_BUSCLK_RTC);
    // /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    // /* System interrupt init*/
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void MF_SystemClock_Config(void) {

}

void MF_Config_Init(void) {
    #warning need check;
    /* Initial PMU */
    // MF_PMU_Init();
    MF_GPIO_Init();
    /* Initial ADC */
    // MF_ADC_Common_Init();
    // MF_ADC_Sampling_Init();
    IncludeDelayMs(100);
    // MF_LCD_Init();
    Config_Init_XTHF_And_XTLF();
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
