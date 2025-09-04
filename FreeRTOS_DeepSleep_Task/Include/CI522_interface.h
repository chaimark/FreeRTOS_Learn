#ifndef CI522_INTERFACE_BUS_H__
#define CI522_INTERFACE_BUS_H__

#define SLA_ADDR 0x50
#define GPIO_SET_H(GPIOX,PINX)          FL_GPIO_SetOutputPin(GPIOX,PINX)      // 设置高电平
#define GPIO_SET_L(GPIOX,PINX)          FL_GPIO_ResetOutputPin(GPIOX,PINX)    // 设置低电平

#define CI522_SDA_GPIO_X               GPIOD
#define CI522_SDA_GPIO_PIN             FL_GPIO_PIN_0
#define CI522_SDA_HIGH                  GPIO_SET_H(GPIOD, FL_GPIO_PIN_0)
#define CI522_SDA_LOW                   GPIO_SET_L(GPIOD, FL_GPIO_PIN_0) 
#define CI522_READ_SDA                  (FL_GPIO_ReadInputPort(GPIOD)&FL_GPIO_PIN_0)
#define CI522_SCL_HIGH                  GPIO_SET_H(GPIOD, FL_GPIO_PIN_1)
#define CI522_SCL_LOW                   GPIO_SET_L(GPIOD, FL_GPIO_PIN_1) 
#define M1_POWER_ON                     GPIO_SET_H(GPIOD, FL_GPIO_PIN_6)
#define M1_POWER_OFF                    GPIO_SET_L(GPIOD, FL_GPIO_PIN_6)  

void  I_CI522_IO_Init(void);
void  I_CI522_IO_Write(unsigned char RegAddr, unsigned char value);
unsigned char  I_CI522_IO_Read(unsigned char RegAddr);
extern void MF_CI522_Init(void);
#endif
