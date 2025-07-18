#include <stdio.h>
#include "SI522A_interface.h"
#include "main.h"

static void SDA_IN(void)  //PA12
{
    FL_GPIO_InitTypeDef    GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_12;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void SDA_OUT(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_12;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;//FL_GPIO_OUTPUT_OPENDRAIN;// LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//	FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_12);
}


//产生IIC起始信号
void IIC_Start_SI522(void) {
    SDA_OUT();     //sda线输出
    IIC_SDA_HIGH;//IIC_SDA=1;	  	  
    IIC_SCL_HIGH;//IIC_SCL=1;
    FL_DelayUs(4);//delay_us(4);
    IIC_SDA_LOW;//IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
    FL_DelayUs(4);//delay_us(4);
    IIC_SCL_LOW;//IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}
//产生IIC停止信号
void IIC_Stop_SI522(void) {
    SDA_OUT();//sda线输出
    IIC_SCL_LOW;//IIC_SCL=0;
    IIC_SDA_LOW;//IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    FL_DelayUs(4);//delay_us(4);
    IIC_SCL_HIGH;//IIC_SCL=1; 
    IIC_SDA_HIGH;//IIC_SDA=1;//发送I2C总线结束信号
    FL_DelayUs(4);//delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void) {
    uint8_t ucErrTime = 0;
    SDA_IN();      //SDA设置为输入 
    IIC_SDA_HIGH; FL_DelayUs(1);//IIC_SDA=1;delay_us(1);	
    IIC_SCL_HIGH; FL_DelayUs(1);//IIC_SCL=1;delay_us(1);	 
    while (IIC_READ_SDA) {
        ucErrTime++;
        if (ucErrTime > 250) {
            IIC_Stop_SI522();
            return 1;
        }
    }
    IIC_SCL_LOW;//IIC_SCL=0;//时钟输出0 	   
    return 0;
}
//产生ACK应答
void IIC_Ack(void) {
    IIC_SCL_LOW;//IIC_SCL=0;
    SDA_OUT();
    IIC_SDA_LOW;//IIC_SDA=0;
    FL_DelayUs(2);//delay_us(2);
    IIC_SCL_HIGH;//IIC_SCL=1;
    FL_DelayUs(2);//delay_us(2);
    IIC_SCL_LOW;//IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void) {
    IIC_SCL_LOW;//IIC_SCL=0;
    SDA_OUT();
    IIC_SDA_HIGH;//IIC_SDA=1;
    FL_DelayUs(2);//delay_us(2);
    IIC_SCL_HIGH;//IIC_SCL=1;
    FL_DelayUs(2);//delay_us(2);
    IIC_SCL_LOW;//IIC_SCL=0;
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd) {
    uint8_t t;
    SDA_OUT();
    IIC_SCL_LOW;//IIC_SCL=0;//拉低时钟开始数据传输
    for (t = 0; t < 8; t++) {
//        IIC_SDA=(txd&0x80)>>7;
        if ((txd & 0x80) >> 7)
            IIC_SDA_HIGH;//IIC_SDA=1;
        else
            IIC_SDA_LOW;//IIC_SDA=0;
        txd <<= 1;
        FL_DelayUs(2);//delay_us(2);   //对TEA5767这三个延时都是必须的
        IIC_SCL_HIGH;//IIC_SCL=1;
        FL_DelayUs(2);//delay_us(2); 
        IIC_SCL_LOW;//IIC_SCL=0;	
        FL_DelayUs(2);//delay_us(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack) {
    unsigned char i, receive = 0;
    SDA_IN();//SDA设置为输入
    for (i = 0; i < 8; i++) {
        IIC_SCL_LOW;//IIC_SCL=0; 
        FL_DelayUs(2);//delay_us(2);
        IIC_SCL_HIGH;//IIC_SCL=1;
        receive <<= 1;
        if (IIC_READ_SDA)receive++;
        FL_DelayUs(1);//delay_us(1); 
    }
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
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
    // IIC_Init();
}

void SI522A_I2C_LL_WriteRawRC(unsigned char RegAddr, unsigned char value) {
    IIC_Start_SI522();
    IIC_Send_Byte(SLA_ADDR);     //发送写器件指令	 
    IIC_Wait_Ack();
    IIC_Send_Byte(RegAddr);      //发送寄存器地址
    IIC_Wait_Ack();
    IIC_Send_Byte(value);        //发送值					   
    IIC_Wait_Ack();
    IIC_Stop_SI522();            //产生一个停止条件 	
}
extern void IIC_Start(void);
unsigned char SI522A_I2C_LL_ReadRawRC(unsigned char RegAddr) {
    uint8_t RegVal = 0;
    IIC_Start_SI522();
    IIC_Send_Byte(SLA_ADDR);        //发送写器件指令	 
    RegVal = IIC_Wait_Ack();
    IIC_Send_Byte(RegAddr);         //发送寄存器地址
    RegVal = IIC_Wait_Ack();
    IIC_Start();                    //重新启动
    IIC_Send_Byte(SLA_ADDR + 1);    //发送读器件指令	
    RegVal = IIC_Wait_Ack();
    RegVal = IIC_Read_Byte(0);      //读取一个字节,不继续再读,发送NAK 	  	
    IIC_Stop_SI522();               //产生一个停止条件 	
    return RegVal;
}

void I_SI522A_IO_Init(void) {
    SI522A_I2C_LL_Init();
    //SI522A_SPI_LL_Init();
}
void I_SI522A_IO_Write(unsigned char RegAddr, unsigned char value) {
    SI522A_I2C_LL_WriteRawRC(RegAddr, value);
    //SI522A_SPI_LL_WriteRawRC(RegAddr,value);
}
unsigned char I_SI522A_IO_Read(unsigned char RegAddr) {
    return SI522A_I2C_LL_ReadRawRC(RegAddr);
    // return SI522A_SPI_LL_ReadRawRC(RegAddr);
}

void MF_SI522A_Init(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct;
    // PB0 M1_SI522A_EN
    GPIO_InitStruct.pin = FL_GPIO_PIN_0;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    M1_POWER_OFF;
    // PB1 SI522A_SDA
    GPIO_InitStruct.pin = FL_GPIO_PIN_1;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_SET_H(GPIOB, FL_GPIO_PIN_1);
    // PD2 SI522A_SCL
    GPIO_InitStruct.pin = FL_GPIO_PIN_2;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    GPIO_SET_H(GPIOD, FL_GPIO_PIN_2);
}

