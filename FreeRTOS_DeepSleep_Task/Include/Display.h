#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Display_LCD_Lib.h"

extern void LCD_MenuPage_Change(char UserPage);
extern void ShowHomePage(void);
extern void ShowClosePage(void);
extern void ShowOpenPage(void);
extern void ShowSignal(void);
extern void ShowWait(bool isWriteEeprom);
extern void Show_M1_Check_ErrerCode(uint8_t ShowID);
extern void Show_UpCode_Start(void);
extern void TesTLCD(void);
extern void Display__Module_Steep(unsigned char steep, unsigned char status);
#endif
