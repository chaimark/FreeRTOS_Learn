#include <stdio.h>
#include "SI522A_interface.h"
#include "main.h"

void MF_SI522A_Init(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct;
    // PD6 M1_SI522A_EN
    GPIO_InitStruct.pin = FL_GPIO_PIN_6;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    M1_POWER_OFF;
    // PD0 SI522A_SDA
    GPIO_InitStruct.pin = SI522A_SDA_GPIO_PIN;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(SI522A_SDA_GPIO_X, &GPIO_InitStruct);
    // PD1 SI522A_SCL
    GPIO_InitStruct.pin = FL_GPIO_PIN_1;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void SI522A_SDA_IN(void) {
    //PB1
    FL_GPIO_InitTypeDef    GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = SI522A_SDA_GPIO_PIN;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(SI522A_SDA_GPIO_X, &GPIO_InitStruct);
}

static void SI522A_SDA_OUT(void) {
    // PB1
    FL_GPIO_InitTypeDef    GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = SI522A_SDA_GPIO_PIN;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(SI522A_SDA_GPIO_X, &GPIO_InitStruct);
}

//产生IIC起始信号
void SI522_IIC_Start(void) {
    SI522A_SDA_OUT();
    SI522_SDA_HIGH;
    SI522_SCL_HIGH;
    FL_DelayUs(4);
    SI522_SDA_LOW;
    FL_DelayUs(4);
    SI522_SCL_LOW;
}
//产生IIC停止信号
void SI522_IIC_Stop(void) {
    SI522A_SDA_OUT();
    SI522_SCL_LOW;
    SI522_SDA_LOW;
    FL_DelayUs(4);
    SI522_SCL_HIGH;
    SI522_SDA_HIGH;
    FL_DelayUs(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t SI522_IIC_Wait_Ack(void) {
    uint8_t ucErrTime = 0;
    SI522A_SDA_IN();      //SDA设置为输入 
    SI522_SDA_HIGH;
    FL_DelayUs(1);
    SI522_SCL_HIGH;
    FL_DelayUs(1);
    while (SI522_READ_SDA) {
        ucErrTime++;
        if (ucErrTime > 250) {
            SI522_IIC_Stop();
            return 1;
        }
    }
    SI522_SCL_LOW;
    return 0;
}
//产生ACK应答
void SI522_IIC_Ack(void) {
    SI522_SCL_LOW;
    SI522A_SDA_OUT();
    SI522_SDA_LOW;
    FL_DelayUs(2);
    SI522_SCL_HIGH;
    FL_DelayUs(2);
    SI522_SCL_LOW;
}
//不产生ACK应答		    
void SI522_IIC_NAck(void) {
    SI522_SCL_LOW;
    SI522A_SDA_OUT();
    SI522_SDA_HIGH;
    FL_DelayUs(2);
    SI522_SCL_HIGH;
    FL_DelayUs(2);
    SI522_SCL_LOW;
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void SI522_IIC_Send_Byte(uint8_t txd) {
    uint8_t t;
    SI522A_SDA_OUT();
    SI522_SCL_LOW;
    for (t = 0; t < 8; t++) {
        if ((txd & 0x80) >> 7)
            SI522_SDA_HIGH;
        else
            SI522_SDA_LOW;
        txd <<= 1;
        FL_DelayUs(2);
        SI522_SCL_HIGH;
        FL_DelayUs(2);
        SI522_SCL_LOW;
        FL_DelayUs(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t SI522_IIC_Read_Byte(unsigned char ack) {
    unsigned char i, receive = 0;
    SI522A_SDA_IN();//SDA设置为输入
    for (i = 0; i < 8; i++) {
        SI522_SCL_LOW;
        FL_DelayUs(2);
        SI522_SCL_HIGH;
        receive <<= 1;
        if (SI522_READ_SDA)receive++;
        FL_DelayUs(1);
    }
    if (!ack)
        SI522_IIC_NAck();//发送nACK
    else
        SI522_IIC_Ack(); //发送ACK   
    return receive;
}

void SI522A_I2C_LL_Init() {
    // GPIO_InitTypeDef GPIO_InitStructure;
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	//CE(NRST)
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);
    // GPIO_SetBits(GPIOA, GPIO_Pin_2);
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	//IRQ
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  		//
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_InitStructure.GPIO_PuPd = IRQMODE;     //新增修改
    // GPIO_Init(GPIOA, &GPIO_InitStructure);
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;	//SPI1的SCK和MOSI，排除干扰
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  		//PA5/7推挽输出 
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);
    // GPIO_ResetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_7);  	//PA5/6/7下拉
    // SI522_IIC_Init();
}

void SI522A_I2C_LL_WriteRawRC(unsigned char RegAddr, unsigned char value) {
    SI522_IIC_Start();
    SI522_IIC_Send_Byte(SI522_SLA_ADDR);     //发送写器件指令	 
    SI522_IIC_Wait_Ack();
    SI522_IIC_Send_Byte(RegAddr);      //发送寄存器地址
    SI522_IIC_Wait_Ack();
    SI522_IIC_Send_Byte(value);        //发送值					   
    SI522_IIC_Wait_Ack();
    SI522_IIC_Stop();            //产生一个停止条件 	
}
unsigned char SI522A_I2C_LL_ReadRawRC(unsigned char RegAddr) {
    uint8_t RegVal = 0;
    SI522_IIC_Start();
    SI522_IIC_Send_Byte(SI522_SLA_ADDR);        //发送写器件指令	 
    RegVal = SI522_IIC_Wait_Ack();
    SI522_IIC_Send_Byte(RegAddr);               //发送寄存器地址
    RegVal = SI522_IIC_Wait_Ack();
    SI522_IIC_Start();                          //重新启动
    SI522_IIC_Send_Byte(SI522_SLA_ADDR + 1);    //发送读器件指令	
    RegVal = SI522_IIC_Wait_Ack();
    RegVal = SI522_IIC_Read_Byte(0);    //读取一个字节,不继续再读,发送NAK 	  	
    SI522_IIC_Stop();                   //产生一个停止条件 	
    return RegVal;
}

void I_SI522A_IO_Init(void) {
    SI522A_I2C_LL_Init();
}
void I_SI522A_IO_Write(unsigned char RegAddr, unsigned char value) {
    SI522A_I2C_LL_WriteRawRC(RegAddr, value);
}
unsigned char I_SI522A_IO_Read(unsigned char RegAddr) {
    return SI522A_I2C_LL_ReadRawRC(RegAddr);
}
