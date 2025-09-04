#include <string.h>
#include "fm33lc0xx_fl.h"
#include "Display_LCD_Lib.h"
#include "Main.h"
#include "RTC.h"
#include "../Interflow/PublicLib_No_One.h"
#include "../Interflow/NumberBaseLib.h"
#include "../Interflow/StrLib.h"
#include "../Interflow/AT24CXXDataLoader.h"

unsigned int Display_Buffer[8] = {0};
#define LCD_BIT_SET(data, bit, Linght)  do{ \
    if (Linght) ((data) |= (0x1 << (bit))); \
    else ((data) &= ~(0x1 << (bit))); \
}while (0)

void LcdDisplayAll(bool state) {
    if (state == true) {
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG0, 0xffffffff);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG1, 0xffffffff);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG2, 0xffffffff);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG3, 0xffffffff);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG4, 0xffffffff);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG5, 0xffffffff);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG6, 0xffffffff);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG7, 0xffffffff);
    } else {
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG0, 0x00000000);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG1, 0x00000000);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG2, 0x00000000);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG3, 0x00000000);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG4, 0x00000000);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG5, 0x00000000);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG6, 0x00000000);
        FL_LCD_WriteData(LCD, FL_LCD_DATA_REG7, 0x00000000);
    }
}

//0~9, A,B, C, D, E, F, 空
const uint8_t FONT_CODE_TABLE[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,
    0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x00,
    0x7D, 0x38, 0x37, 0x3F, 0x73, 0x3E, 0x6D,//G,L,N,O,P,U,S
    0x5c, 0x50, 0x01, 0x40, 0x08, 0x41, 0x09, 0x48, 0x49,0x00,0x54,0x76,0x47,0x40,0x76,//xiao_o,xiao_r,LINE1,LINE2,LINE3,LINE12,LINE13,LINE23,LINE123,SPACE,xiao_n,da_h,fan_f,mid_line,xiao_h
};

void LcdDisplayRefresh(void) {
    FL_LCD_WriteData(LCD, FL_LCD_DATA_REG0, Display_Buffer[0]);
    FL_LCD_WriteData(LCD, FL_LCD_DATA_REG1, Display_Buffer[1]);
    FL_LCD_WriteData(LCD, FL_LCD_DATA_REG2, Display_Buffer[2]);
    FL_LCD_WriteData(LCD, FL_LCD_DATA_REG3, Display_Buffer[3]);
    FL_LCD_WriteData(LCD, FL_LCD_DATA_REG4, Display_Buffer[4]);
    FL_LCD_WriteData(LCD, FL_LCD_DATA_REG5, Display_Buffer[5]);
    FL_LCD_WriteData(LCD, FL_LCD_DATA_REG6, Display_Buffer[6]);
    FL_LCD_WriteData(LCD, FL_LCD_DATA_REG7, Display_Buffer[7]);
}

void ClearDisplayAll(void) {
    uint8_t i = 0;
    for (i = 0; i < 8; i++)
        Display_Buffer[i] = 0;
    LcdDisplayRefresh();
}
// true: 点亮; false: 熄灭
void LcdDisplayBIT(uint32_t x, uint32_t y, bool value) {
    LCD_BIT_SET(Display_Buffer[x - 1], y - 1, (value & 0x01));
    LcdDisplayRefresh();
}

static uint8_t NumberAddrForBufAboutID[8] = {9,8,7,6,5,4,3,2};
void LCD_Num_Data(uint8_t Show, uint8_t Num) {
    if ((Num < 1) || (Num >= 9)) {
        return;
    }
    Num--;
    uint8_t Result;
    Result = FONT_CODE_TABLE[Show];
    Num = 8 - Num;
    LCD_BIT_SET(Display_Buffer[0], NumberAddrForBufAboutID[Num - 1], (Result & 0x01)); // A
    LCD_BIT_SET(Display_Buffer[1], NumberAddrForBufAboutID[Num - 1], (Result & 0x20)); // F
    LCD_BIT_SET(Display_Buffer[2], NumberAddrForBufAboutID[Num - 1], (Result & 0x02)); // B
    LCD_BIT_SET(Display_Buffer[3], NumberAddrForBufAboutID[Num - 1], (Result & 0x40)); // G
    LCD_BIT_SET(Display_Buffer[4], NumberAddrForBufAboutID[Num - 1], (Result & 0x10)); // E
    LCD_BIT_SET(Display_Buffer[5], NumberAddrForBufAboutID[Num - 1], (Result & 0x04)); // C
    LCD_BIT_SET(Display_Buffer[6], NumberAddrForBufAboutID[Num - 1], (Result & 0x08)); // D
}

void LCD_SetDotPx(uint8_t DotNum, bool isShow) {
    if ((DotNum < 1) || (DotNum >= 8)) {
        return;
    }
    DotNum--;
    DotNum = 8 - DotNum;
    LCD_BIT_SET(Display_Buffer[7], NumberAddrForBufAboutID[DotNum - 2], isShow);
}

/*******************end function***************************************************/

/**
  * @brief  LCD Initialization function
  * @param  void
  * @retval None
  */
void MF_LCD_Init(void) {
#define LCD_GPIOA_PINX FL_GPIO_PIN_0 | FL_GPIO_PIN_1 | FL_GPIO_PIN_2 | FL_GPIO_PIN_3 | FL_GPIO_PIN_4 | FL_GPIO_PIN_5 | FL_GPIO_PIN_6 | FL_GPIO_PIN_7 | FL_GPIO_PIN_8 | FL_GPIO_PIN_9 | FL_GPIO_PIN_10
#define LCD_GPIOB_PINX FL_GPIO_PIN_4 | FL_GPIO_PIN_5 | FL_GPIO_PIN_6 | FL_GPIO_PIN_7 | FL_GPIO_PIN_8 | FL_GPIO_PIN_9 | FL_GPIO_PIN_10
    FL_GPIO_InitTypeDef gpioInitStruction = {0};
    FL_LCD_InitTypeDef lcdInitStruction;

    // GPIO Init
    gpioInitStruction.mode = FL_GPIO_MODE_ANALOG;
    gpioInitStruction.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    gpioInitStruction.pull = FL_DISABLE;

    gpioInitStruction.pin = LCD_GPIOA_PINX;
    FL_GPIO_Init(GPIOA, &gpioInitStruction);

    gpioInitStruction.pin = LCD_GPIOB_PINX;
    FL_GPIO_Init(GPIOB, &gpioInitStruction);

    // LCD Init
    lcdInitStruction.biasCurrent = FL_LCD_BIAS_CURRENT_HIGH;
    lcdInitStruction.biasMode = FL_LCD_BIAS_MODE_4BIAS;
    lcdInitStruction.biasVoltage = FL_LCD_BIAS_VOLTAGE_LEVEL15;
    lcdInitStruction.COMxNum = FL_LCD_COM_NUM_8COM;
    lcdInitStruction.waveform = FL_LCD_WAVEFORM_TYPEB;
    lcdInitStruction.displayFreq = 64;
    lcdInitStruction.mode = FL_LCD_DRIVER_MODE_INNER_RESISTER;
    FL_LCD_Init(LCD, &lcdInitStruction);

    // COM and SEG Init
    FL_LCD_EnableCOMEN(LCD, FL_LCD_COMEN_COM0);
    FL_LCD_EnableCOMEN(LCD, FL_LCD_COMEN_COM1);
    FL_LCD_EnableCOMEN(LCD, FL_LCD_COMEN_COM2);
    FL_LCD_EnableCOMEN(LCD, FL_LCD_COMEN_COM3);
    FL_LCD_EnableCOMEN(LCD, FL_LCD_COMEN_COM4);
    FL_LCD_EnableCOMEN(LCD, FL_LCD_COMEN_COM5);
    FL_LCD_EnableCOMEN(LCD, FL_LCD_COMEN_COM6);
    FL_LCD_EnableCOMEN(LCD, FL_LCD_COMEN_COM7);

    FL_LCD_EnableSEGEN0(LCD, FL_LCD_SEGEN0_SEG0);
    FL_LCD_EnableSEGEN0(LCD, FL_LCD_SEGEN0_SEG1);
    FL_LCD_EnableSEGEN0(LCD, FL_LCD_SEGEN0_SEG2);
    FL_LCD_EnableSEGEN0(LCD, FL_LCD_SEGEN0_SEG3);
    FL_LCD_EnableSEGEN0(LCD, FL_LCD_SEGEN0_SEG4);
    FL_LCD_EnableSEGEN0(LCD, FL_LCD_SEGEN0_SEG5);
    FL_LCD_EnableSEGEN0(LCD, FL_LCD_SEGEN0_SEG6);
    FL_LCD_EnableSEGEN0(LCD, FL_LCD_SEGEN0_SEG7);
    FL_LCD_EnableSEGEN0(LCD, FL_LCD_SEGEN0_SEG8);
    FL_LCD_EnableSEGEN0(LCD, FL_LCD_SEGEN0_SEG9);

    FL_LCD_Enable(LCD);
    ClearDisplayAll();
}
