#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Display_LCD_Lib.h"

extern void LCD_MenuPage_Change(void);
extern void ShowHomePage(void);
extern void ShowClosePage(void);
extern void ShowOpenPage(void);
extern void ShowSignal(bool IsLinght);
extern void ShowWait(void);

#endif
