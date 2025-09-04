#ifndef __LPUART_0_And_1_LIB_H__
#define __LPUART_0_And_1_LIB_H__
#include "fm33lc0xx_fl.h"
#include "Define.h"

#define LPUart0DisableIRQ        NVIC_DisableIRQ(LPUART0_IRQn)   // 关闭中断使能
#define LPUart0EnableIRQ         NVIC_EnableIRQ(LPUART0_IRQn)    // 开启中断使能
#define Uart1DisableIRQ          NVIC_DisableIRQ(UART1_IRQn)     // 关闭中断使能
#define Uart1EnableIRQ           NVIC_EnableIRQ(UART1_IRQn)      // 开启中断使能

#define Infrared_POWER_ON               GPIO_SET_H(GPIOA, FL_GPIO_PIN_15)
#define Infrared_POWER_OFF              GPIO_SET_L(GPIOA, FL_GPIO_PIN_15) 

extern void OpenUartOfLightOrM1(void);
extern void LPUART0_Init(void);
extern void MF_UART1_Init(void);
extern void CloseLPUart0TTL(void);
extern void CloseUart1TTL(void);
extern void MF_LPUART0_Interrupt_Init(void);
extern void MF_UART1_Interrupt_Init(void);
extern void LPUART0_Send(unsigned char * txBuffer, int length);
extern void UART1_Send(unsigned char * txBuffer, int length);
extern void Clr_LPUart0_RxBuffer(void);
extern void Clr_Uart1_RxBuffer(void);
#endif
