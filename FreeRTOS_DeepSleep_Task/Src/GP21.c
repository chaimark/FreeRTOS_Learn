#include "main.h"
#include "GP21.h"
#include "Define.h"

unsigned int GP21_TEST_DELAY_TIME_uS = 0x1600;

void GP21_Write_Reg16(unsigned int DA2, unsigned char Count) {
    unsigned char i;
    GP21_NSS_L;
    for (i = 0; i < Count; i++) {
        GP21_SCLK_H;
        GP21_SIMO_L;
        if ((0x08000 & DA2) != 0) {
            GP21_SIMO_H;
        }
        DA2 = DA2 << 1;
        GP21_SCLK_L;
    }
}

unsigned int GP21_Read_Reg16(void) {
    unsigned int DA2 = 0;
    unsigned char i;
    GP21_NSS_L;
    for (i = 0; i < 16; i++) {
        GP21_SCLK_H;
        DA2 = DA2 << 1;
        if (GP21_SOMI_VALUE != 0) {
            DA2++;
        }
        GP21_SCLK_L;
    }
    return DA2;
}

unsigned long int GP21_Read_Reg32(void) {
    unsigned long int DA2 = 0;
    unsigned char i;
    GP21_NSS_L;
    for (i = 0; i < 32; i++) {
        GP21_SCLK_H;
        DA2 = DA2 << 1;
        if (GP21_SOMI_VALUE != 0) {
            DA2++;
        }
        GP21_SCLK_L;
    }
    return DA2;
}

unsigned long int GP21_Get_Data32(void) {
    unsigned long int  data1;
    data1 = GP21_Read_Reg32();
    return data1;
}

void GP21_SPI_Register(unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4) {
    unsigned int data;
    data = (a1 << 8) + a2;
    GP21_Write_Reg16(data, 16);
    data = (a3 << 8) + a4;
    GP21_Write_Reg16(data, 16);
}

void GP21_Write_Reg_DelayTime(unsigned int delayTime) {
    GP21_Write_Reg16(delayTime, 16);
}

void GP21_Reset(void) {
    GP21_RST_H;
    FL_DelayUs(100);
    GP21_NSS_H;
    FL_DelayUs(2);
    GP21_NSS_L;
    GP21_Write_Reg16(0X5000, 8);
    GP21_NSS_H;
}

void GP21_Spi_Init(unsigned int delayTime) {
    GP21_NSS_H;
    FL_DelayUs(1);
    GP21_NSS_L;
    GP21_SPI_Register(0X80, 0XF3, 0X87, 0X68);
    GP21_NSS_H;

    FL_DelayUs(1);
    GP21_NSS_L;
    GP21_SPI_Register(0X81, 0X21, 0X44, 0X00);
    GP21_NSS_H;

    FL_DelayUs(1);
    GP21_NSS_L;
    GP21_Write_Reg16(0X82E0, 16);

    GP21_Write_Reg_DelayTime(delayTime);
    GP21_NSS_H;
    FL_DelayUs(1);

    GP21_NSS_L;
    GP21_SPI_Register(0X83, 0X10, 0X00, 0X00);
    GP21_NSS_H;

    FL_DelayUs(1);
    GP21_NSS_L;
    GP21_SPI_Register(0X84, 0X20, 0X00, 0X00);
    GP21_NSS_H;

    FL_DelayUs(1);
    GP21_NSS_L;
    GP21_SPI_Register(0X85, 0X48, 0X00, 0X00);
    GP21_NSS_H;
}

void Delay_GP21_DelayTime(unsigned int delayTime) {
    GP21_NSS_L;
    GP21_Write_Reg16(0X82E0, 16);
    GP21_Write_Reg_DelayTime(delayTime);
    GP21_NSS_H;
}

void GP21_Goto_Realy(void) {
    GP21_PWR_HIGH;
    IncludeDelayMs(20);
    GP21_Reset();
    GP21_Spi_Init(GP21_TEST_DELAY_TIME_uS);
    GP21_NSS_H;
}

void GP21_Goto_Low_Pwr(void) {
    GP21_NSS_H;
    EN761_LOW;
    GP21_PWR_LOW;
    GP21_RST_L;
    GP21_NSS_L;
    GP21_SCLK_L;
    GP21_SIMO_L;
}

unsigned int GP21_Read_Status(void) {
    unsigned int r = 0;
    r = GP21_Read_Reg16();
    return r;
}

void GP21_GPIO_Init(void) {
#warning "GP2 Config";
// FL_GPIO_InitTypeDef gpioInitStruction = {0};
// // PA8_GP2_SO
// gpioInitStruction.pin = FL_GPIO_PIN_8;
// gpioInitStruction.mode = FL_GPIO_MODE_INPUT;
// gpioInitStruction.outputType = FL_GPIO_OUTPUT_PUSHPULL;
// gpioInitStruction.pull = FL_DISABLE;
// FL_GPIO_Init(GPIOA, &gpioInitStruction);
// // PA9_GP2_RST    PA10_GP2_SI 
// gpioInitStruction.pin = FL_GPIO_PIN_9 | FL_GPIO_PIN_10;
// gpioInitStruction.mode = FL_GPIO_MODE_OUTPUT;
// gpioInitStruction.outputType = FL_GPIO_OUTPUT_PUSHPULL;
// gpioInitStruction.pull = FL_DISABLE;
// FL_GPIO_Init(GPIOA, &gpioInitStruction);
// GPIO_SET_L(GPIOA, FL_GPIO_PIN_9 | FL_GPIO_PIN_10);
// // PB3_GP2_SCK    PB4_GP2_SSN
// gpioInitStruction.pin = FL_GPIO_PIN_3 | FL_GPIO_PIN_4;
// gpioInitStruction.mode = FL_GPIO_MODE_OUTPUT;
// gpioInitStruction.outputType = FL_GPIO_OUTPUT_PUSHPULL;
// gpioInitStruction.pull = FL_DISABLE;
// FL_GPIO_Init(GPIOB, &gpioInitStruction);
// GPIO_SET_L(GPIOB, FL_GPIO_PIN_3 | FL_GPIO_PIN_4);
// // PB5_GP2_INT
// gpioInitStruction.pin = FL_GPIO_PIN_5;
// gpioInitStruction.mode = FL_GPIO_MODE_INPUT;
// gpioInitStruction.outputType = FL_GPIO_OUTPUT_PUSHPULL;
// gpioInitStruction.pull = FL_DISABLE;
// FL_GPIO_Init(GPIOB, &gpioInitStruction);
// //PB9_GP21_EN761    PB10_GP2_PWR    PB12_ACLK
// gpioInitStruction.pin = FL_GPIO_PIN_9 | FL_GPIO_PIN_10 | FL_GPIO_PIN_12;
// gpioInitStruction.mode = FL_GPIO_MODE_OUTPUT;
// gpioInitStruction.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
// gpioInitStruction.pull = FL_DISABLE;
// FL_GPIO_Init(GPIOB, &gpioInitStruction);
// GPIO_SET_L(GPIOB, FL_GPIO_PIN_9 | FL_GPIO_PIN_10 | FL_GPIO_PIN_12);
}

