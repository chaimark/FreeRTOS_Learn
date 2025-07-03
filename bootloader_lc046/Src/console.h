#ifndef __CONSOLE_H
#define __CONSOLE_H

//#define CONSOLE_UART  LPUART0
//#define CONSOLE_UART  LPUART1
#define CONSOLE_UART  UART4

#define RS485_RECIVE  	FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_4)
#define RS485_SEND      FL_GPIO_SetOutputPin (GPIOA, FL_GPIO_PIN_4)

void console_init(void);

#endif 

