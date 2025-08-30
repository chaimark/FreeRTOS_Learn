/**
******************************************************************************
* @file           : mf_config.c
* @brief          : MCU FUNCTION CONFIG               
******************************************************************************
* @attention
*
* Copyright (c) [2021] [Fudan Microelectronics]
* THIS SOFTWARE is licensed under Mulan PSL v2.
* can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "mf_config.h"

/* Private function prototypes -----------------------------------------------*/

 
/**
  * @brief  GPIO Initialization function
  * @param  void
  * @retval None
  */
void MF_GPIO_Init(void)
{
    FL_GPIO_InitTypeDef    GPIO_InitStruct;
/*
    GPIO_InitStruct.pin = FL_GPIO_PIN_0;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_1;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
*/
    GPIO_InitStruct.pin = FL_GPIO_PIN_4;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
/*
    GPIO_InitStruct.pin = FL_GPIO_PIN_5;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_15;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_4;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_7;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_12;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_0;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_1;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_6;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_7;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_4;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_5;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);    
*/

//	GPIO_InitStruct.pin = FL_GPIO_PIN_4;
//    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
//    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
//    GPIO_InitStruct.pull = FL_DISABLE;
//    GPIO_InitStruct.remapPin = FL_DISABLE;
//    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//    GPIO_InitStruct.pin = FL_GPIO_PIN_5;
//    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
//    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
//    GPIO_InitStruct.pull = FL_DISABLE;
//    GPIO_InitStruct.remapPin = FL_DISABLE;
//    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//    GPIO_InitStruct.pin = FL_GPIO_PIN_6;
//    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
//    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
//    GPIO_InitStruct.pull = FL_DISABLE;
//    GPIO_InitStruct.remapPin = FL_DISABLE;
//    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//    GPIO_InitStruct.pin = FL_GPIO_PIN_7;
//    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
//    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
//    GPIO_InitStruct.pull = FL_DISABLE;
//    GPIO_InitStruct.remapPin = FL_DISABLE;
//    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);


//	GPIO_InitStruct.pin = FL_GPIO_PIN_4;
//    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
//    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
//    GPIO_InitStruct.pull = FL_DISABLE;
//    GPIO_InitStruct.remapPin = FL_DISABLE;
//    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//    GPIO_InitStruct.pin = FL_GPIO_PIN_5;
//    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
//    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
//    GPIO_InitStruct.pull = FL_DISABLE;
//    GPIO_InitStruct.remapPin = FL_DISABLE;
//    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//	GPIO_InitStruct.pin = FL_GPIO_PIN_6;
//    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
//    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
//    GPIO_InitStruct.pull = FL_DISABLE;
//    GPIO_InitStruct.remapPin = FL_DISABLE;
//    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);


//	FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_5);
//    FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_4);
//    FL_GPIO_ResetOutputPin(GPIOD, FL_GPIO_PIN_6);
}
 
/**
  * @brief  RCC_XTLF_IO Initialization function
  * @param  void
  * @retval None
  */
void MF_RCC_XTLF_IO_Init(void)
{
    FL_GPIO_InitTypeDef    GPIO_InitStruct;

    GPIO_InitStruct.pin = FL_GPIO_PIN_9;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_10;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}
 
/**
  * @brief  SPI1 Initialization function
  * @param  void
  * @retval None
  */
void MF_SPI1_Init(void)
{
    FL_GPIO_InitTypeDef    GPIO_InitStruct;

    FL_SPI_InitTypeDef    SPI1_InitStruct;

    GPIO_InitStruct.pin = FL_GPIO_PIN_8;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_9;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_10;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_11;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    SPI1_InitStruct.transferMode = FL_SPI_TRANSFER_MODE_FULL_DUPLEX;
    SPI1_InitStruct.mode = FL_SPI_WORK_MODE_MASTER;
    SPI1_InitStruct.dataWidth = FL_SPI_DATA_WIDTH_8B;
    SPI1_InitStruct.clockPolarity = FL_SPI_POLARITY_NORMAL;
    SPI1_InitStruct.clockPhase = FL_SPI_PHASE_EDGE1;
    SPI1_InitStruct.softControl = FL_ENABLE;
    SPI1_InitStruct.baudRate = FL_SPI_BAUDRATE_DIV8;
    SPI1_InitStruct.bitOrder = FL_SPI_BIT_ORDER_MSB_FIRST;

    FL_SPI_Init(SPI1, &SPI1_InitStruct);    
}
/**
  * @brief  UART4 Initialization function
  * @param  void
  * @retval None
  */
void MF_UART4_Init(void)
{
    FL_GPIO_InitTypeDef    GPIO_InitStruct;

    FL_UART_InitTypeDef    UART4_InitStruct;

    GPIO_InitStruct.pin = FL_GPIO_PIN_2;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;

    FL_GPIO_Init( GPIOB, &GPIO_InitStruct );

    GPIO_InitStruct.pin = FL_GPIO_PIN_3;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;

    FL_GPIO_Init( GPIOB, &GPIO_InitStruct );

    UART4_InitStruct.clockSrc = NULL;
    UART4_InitStruct.baudRate = 115200;
    UART4_InitStruct.dataWidth = FL_UART_DATA_WIDTH_8B;
    UART4_InitStruct.stopBits = FL_UART_STOP_BIT_WIDTH_1B;
    UART4_InitStruct.parity = FL_UART_PARITY_NONE;
    UART4_InitStruct.transferDirection = FL_UART_DIRECTION_TX_RX;

    FL_UART_Init(UART4, &UART4_InitStruct);    
}
 
/**
  * @brief  UART4 Interrupt Initialization function
  * @param  void
  * @retval None
  */
void MF_UART4_Interrupt_Init(void)
{
    FL_UART_ClearFlag_RXBuffFull(UART4);
    FL_UART_EnableIT_RXBuffFull(UART4);  
}
 
/**
  * @brief  UART5 Initialization function
  * @param  void
  * @retval None
  */
void MF_UART5_Init(void)
{
    FL_GPIO_InitTypeDef    GPIO_InitStruct;

    FL_UART_InitTypeDef    UART5_InitStruct;

    GPIO_InitStruct.pin = FL_GPIO_PIN_4;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_5;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    UART5_InitStruct.clockSrc = NULL;
    UART5_InitStruct.baudRate = 115200;
    UART5_InitStruct.dataWidth = FL_UART_DATA_WIDTH_8B;
    UART5_InitStruct.stopBits = FL_UART_STOP_BIT_WIDTH_1B;
    UART5_InitStruct.parity = FL_UART_PARITY_NONE;
    UART5_InitStruct.transferDirection = FL_UART_DIRECTION_TX_RX;

    FL_UART_Init(UART5, &UART5_InitStruct);    
}
 
/**
  * @brief  UART5 Interrupt Initialization function
  * @param  void
  * @retval None
  */
void MF_UART5_Interrupt_Init(void)
{
    FL_UART_ClearFlag_RXBuffFull(UART5);
    FL_UART_EnableIT_RXBuffFull(UART5);  
}
 
/**
  * @brief  IWDT Initialization function
  * @param  void
  * @retval None
  */
void MF_IWDT_Init(void)
{
    FL_IWDT_InitTypeDef    IWDT_InitStruct;

    IWDT_InitStruct.overflowPeriod = FL_IWDT_PERIOD_4000MS;
    IWDT_InitStruct.iwdtWindows = 0;

    FL_IWDT_Init(IWDT, &IWDT_InitStruct);    
}
 
/**
  * @brief  BSTIM32 Initialization function
  * @param  void
  * @retval None
  */
void MF_BSTIM32_Init(void)
{
    FL_BSTIM32_InitTypeDef    BSTIM32_InitStruct;

    BSTIM32_InitStruct.prescaler = 799;
    BSTIM32_InitStruct.autoReload = 799;
    BSTIM32_InitStruct.autoReloadState = FL_ENABLE;
    BSTIM32_InitStruct.clockSource = FL_RCC_BSTIM32_CLK_SOURCE_APB2CLK;

    FL_BSTIM32_Init(BSTIM32, &BSTIM32_InitStruct);    
}
 
/**
  * @brief  BSTIM32 Interrupt Initialization function
  * @param  void
  * @retval None
  */
void MF_BSTIM32_Interrupt_Init(void)
{
    FL_BSTIM32_ClearFlag_Update(BSTIM32);
    FL_BSTIM32_EnableIT_Update(BSTIM32);  
}
 
/**
  * @brief  NVIC Initialization function
  * @param  void
  * @retval None
  */
void MF_NVIC_Init(void)
{
    FL_NVIC_ConfigTypeDef    InterruptConfigStruct;

    // InterruptConfigStruct.preemptPriority = 0x02;
    InterruptConfigStruct.preemptPriority = 0x02;
    FL_NVIC_Init(&InterruptConfigStruct, UART4_IRQn);

    // InterruptConfigStruct.preemptPriority = 0x02;
    // FL_NVIC_Init(&InterruptConfigStruct, BSTIM_IRQn);

	//InterruptConfigStruct.preemptPriority = 0x0002;
	//FL_NVIC_Init(&InterruptConfigStruct, LPUART0_IRQn);    

}


/**
  * @brief  Clock Initialization.
  * @retval None
  */
void MF_Clock_Init(void)
{
    /* Initial XTLF */
    MF_RCC_XTLF_IO_Init();
    FL_RCC_XTLF_SetWorkCurrent(FL_RCC_XTLF_WORK_CURRENT_450NA);    /* 设置工作电流为最大以便快速起振 */
    FL_RCC_XTLF_Enable();

    /* Initial Low Speed Clock */
    FL_RCC_SetLSCLKClockSource(FL_RCC_LSCLK_CLK_SOURCE_XTLF);
    FL_RCC_EnableLSCLKAutoSwitch();

    /* Initial RCHF */
    FL_RCC_RCHF_SetFrequency(FL_RCC_RCHF_FREQUENCY_8MHZ);
    FL_RCC_RCHF_Enable();

    /* Initial PLL */
    FL_RCC_PLL_Disable();
    FL_RCC_PLL_SetClockSource(FL_RCC_PLL_CLK_SOURCE_RCHF);
    FL_RCC_PLL_SetPrescaler(FL_RCC_PLL_PSC_DIV8);
    FL_RCC_PLL_WriteMultiplier(63);
    FL_RCC_PLL_SetOutputMultiplier(FL_RCC_PLL_OUTPUT_X1);
    FL_RCC_PLL_Enable();
    while(FL_RCC_IsActiveFlag_PLLReady() != FL_SET);

    /* Initial System Clock */
    FL_FLASH_SetReadWait(FLASH, FL_FLASH_READ_WAIT_2CYCLE);    /* 设置FLASH读等待为 2 个周期 */
    FL_RCC_SetSystemClockSource(FL_RCC_SYSTEM_CLK_SOURCE_PLL);    /* 设置系统主时钟为 PLL */
    FL_RCC_SetAHBPrescaler(FL_RCC_AHBCLK_PSC_DIV1);
    FL_RCC_SetAPB1Prescaler(FL_RCC_APB1CLK_PSC_DIV1);
    FL_RCC_SetAPB2Prescaler(FL_RCC_APB2CLK_PSC_DIV1);

    SystemCoreClockUpdate(); 
}

void MF_LPUART0_Init(void)
{
    FL_GPIO_InitTypeDef    GPIO_InitStruct;

    FL_LPUART_InitTypeDef    LPUART0_InitStruct;

    GPIO_InitStruct.pin = FL_GPIO_PIN_2;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_ENABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_3;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_ENABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LPUART0_InitStruct.clockSrc = FL_RCC_LPUART_CLK_SOURCE_LSCLK;
    LPUART0_InitStruct.baudRate = FL_LPUART_BAUDRATE_9600;
    LPUART0_InitStruct.dataWidth = FL_LPUART_DATA_WIDTH_8B;
    LPUART0_InitStruct.stopBits = FL_LPUART_STOP_BIT_WIDTH_1B;
    LPUART0_InitStruct.parity = FL_LPUART_PARITY_NONE;
    LPUART0_InitStruct.transferDirection = FL_LPUART_DIRECTION_TX_RX;

    FL_LPUART_Init(LPUART0, &LPUART0_InitStruct);    
}

void MF_LPUART1_Init(void)
{
    FL_GPIO_InitTypeDef    GPIO_InitStruct;

    FL_LPUART_InitTypeDef    LPUART1_InitStruct;

    GPIO_InitStruct.pin = FL_GPIO_PIN_2;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_ENABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = FL_GPIO_PIN_3;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_ENABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    LPUART1_InitStruct.clockSrc = FL_RCC_LPUART_CLK_SOURCE_LSCLK;
    LPUART1_InitStruct.baudRate = FL_LPUART_BAUDRATE_9600;
    LPUART1_InitStruct.dataWidth = FL_LPUART_DATA_WIDTH_8B;
    LPUART1_InitStruct.stopBits = FL_LPUART_STOP_BIT_WIDTH_1B;
    LPUART1_InitStruct.parity = FL_LPUART_PARITY_NONE;
    LPUART1_InitStruct.transferDirection = FL_LPUART_DIRECTION_TX_RX;

    FL_LPUART_Init(LPUART1, &LPUART1_InitStruct);    
}
 
/**
  * @brief  LPUART0 Interrupt Initialization function
  * @param  void
  * @retval None
  */
void MF_LPUART0_Interrupt_Init(void)
{
    FL_LPUART_ClearFlag_RXBuffFull(LPUART0);
    FL_LPUART_EnableIT_RXBuffFull(LPUART0);  
}

void MF_Config_Init(void)
{
    /* Initial GPIO */
    MF_GPIO_Init();

    /* Initial SPI1 */
    // MF_SPI1_Init();

    /* Initial UART5 */
    MF_UART4_Init();
    MF_UART4_Interrupt_Init();

    /* Initial IWDT */
	  MF_IWDT_Init();

	// MF_LPUART0_Init();
	// MF_LPUART0_Interrupt_Init();
	//MF_LPUART1_Init();

    /* Initial BSTIM32 */
    // MF_BSTIM32_Init();
    // MF_BSTIM32_Interrupt_Init();

    /* Initial NVIC */
    MF_NVIC_Init();
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 

}
#endif /* USE_FULL_ASSERT */

/*************************(C) COPYRIGHT Fudan Microelectronics **** END OF FILE*************************/
