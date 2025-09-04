#ifndef SI522A_INTERFACE_BUS_H__
#define SI522A_INTERFACE_BUS_H__
#include "Define.h"
#define SI522_SLA_ADDR 0x50

#define SI522A_SDA_GPIO_X               GPIOD
#define SI522A_SDA_GPIO_PIN             FL_GPIO_PIN_0
#define SI522_SDA_HIGH                  GPIO_SET_H(GPIOD, FL_GPIO_PIN_0)
#define SI522_SDA_LOW                   GPIO_SET_L(GPIOD, FL_GPIO_PIN_0) 
#define SI522_READ_SDA                  (FL_GPIO_ReadInputPort(GPIOD)&FL_GPIO_PIN_0)
#define SI522_SCL_HIGH                  GPIO_SET_H(GPIOD, FL_GPIO_PIN_1)
#define SI522_SCL_LOW                   GPIO_SET_L(GPIOD, FL_GPIO_PIN_1) 
#define M1_POWER_ON                     GPIO_SET_H(GPIOD, FL_GPIO_PIN_6)
#define M1_POWER_OFF                    GPIO_SET_L(GPIOD, FL_GPIO_PIN_6) 

extern void I_SI522A_IO_Init(void);
extern void I_SI522A_IO_Write(unsigned char RegAddr, unsigned char value);
extern unsigned char I_SI522A_IO_Read(unsigned char RegAddr);
extern void MF_SI522A_Init(void);
#endif
