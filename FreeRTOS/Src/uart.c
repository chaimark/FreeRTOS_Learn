#include "uart.h"
#include <stdio.h>
#ifdef __CC_ARM
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE
{ 
    int handle; 
}; 
FILE __stdout;
#endif

//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
    x = x; 
} 

int fputc(int ch, FILE *f)
{
    UART0->TXBUF = ch;
    while (!(UART0->ISR & (0x1 << 0)));
    UART0->ISR |= (0x1 << 0);
    
    return ch;
}

void DebugInit(void)
{
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    FL_UART_InitTypeDef UART_InitStruct = {0};  

    GPIO_InitStruct.pin = FL_GPIO_PIN_13|FL_GPIO_PIN_14;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);  

    UART_InitStruct.clockSrc = FL_RCC_UART1_CLK_SOURCE_APB1CLK;
	UART_InitStruct.baudRate = 115200;								
	UART_InitStruct.dataWidth = FL_UART_DATA_WIDTH_8B;				
	UART_InitStruct.stopBits = FL_UART_STOP_BIT_WIDTH_1B;				
	UART_InitStruct.parity = FL_UART_PARITY_EVEN;					
	UART_InitStruct.transferDirection = FL_UART_DIRECTION_TX;		        
	FL_UART_Init(UART0, &UART_InitStruct);    
}



