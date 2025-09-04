#ifndef _GP21_H
#define _GP21_H

#include "Define.h"
#include "PublicLib_No_One.h"

#define  GP21_SIMO_H                    GPIO_SET_H(GPIOC, FL_GPIO_PIN_2)
#define  GP21_SIMO_L                    GPIO_SET_L(GPIOC, FL_GPIO_PIN_2)
#define  GP21_SCLK_H                    GPIO_SET_H(GPIOC, FL_GPIO_PIN_8)
#define  GP21_SCLK_L                    GPIO_SET_L(GPIOC, FL_GPIO_PIN_8)
#define  GP21_NSS_H                     GPIO_SET_H(GPIOC, FL_GPIO_PIN_7)
#define  GP21_NSS_L                     GPIO_SET_L(GPIOC, FL_GPIO_PIN_7) 
#define  GP21_RST_H                     GPIO_SET_H(GPIOC, FL_GPIO_PIN_0)
#define  GP21_RST_L                     GPIO_SET_L(GPIOC, FL_GPIO_PIN_0) 
#define  GP21_SOMI_VALUE                (FL_GPIO_ReadInputPort(GPIOC)&FL_GPIO_PIN_1)
#define  GP21_INT_VALUE                 (FL_GPIO_ReadInputPort(GPIOC)&FL_GPIO_PIN_9)

#define  GP21_SOMI_L                    GPIO_SET_L(GPIOC, FL_GPIO_PIN_1) 
#define  GP21_INT_L                     GPIO_SET_L(GPIOC, FL_GPIO_PIN_9) 

extern void GP21_Goto_Realy(void);
extern void GP21_Goto_Low_Pwr(void);
extern void GP21_Write_Reg16(unsigned int DA2, unsigned char CISHU);
extern unsigned int GP21_Read_Status(void);
extern unsigned long GP21_Get_Data32(void);
extern void GP21_GPIO_Init(void);

#endif
