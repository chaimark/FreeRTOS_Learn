#ifndef __LPUART_0_And_1_LIB_H__
#define __LPUART_0_And_1_LIB_H__
#include "fm33lc0xx_fl.h"
#include "Define.h"

extern void LPUART_Init(LPUART_Type * LPUARTx);
extern void MF_LPUART0_Interrupt_Init(void);
extern void MF_LPUART1_Interrupt_Init(void);
extern void LPUART0_Send(unsigned char * txBuffer, int length);
extern void LPUART1_Send(unsigned char * txBuffer, int length);
extern void Clr_LPUart0_RxBuffer(void);
extern void Clr_LPUart1_RxBuffer(void);
#endif
