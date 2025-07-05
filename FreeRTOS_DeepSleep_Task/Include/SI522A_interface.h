#ifndef SI522A_INTERFACE_BUS_H__
#define SI522A_INTERFACE_BUS_H__

#define SLA_ADDR 0x50

#define  IIC_SDA_HIGH          FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_12)
#define  IIC_SDA_LOW           FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_12) 

#define  IIC_READ_SDA         (FL_GPIO_ReadInputPort(GPIOA)&FL_GPIO_PIN_12)

#define  IIC_SCL_HIGH          FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_11)
#define  IIC_SCL_LOW           FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_11)  

extern void I_SI522A_IO_Init(void);
extern void I_SI522A_IO_Write(unsigned char RegAddr, unsigned char value);
extern unsigned char I_SI522A_IO_Read(unsigned char RegAddr);
extern void MF_SI522A_Init(void);
#endif
