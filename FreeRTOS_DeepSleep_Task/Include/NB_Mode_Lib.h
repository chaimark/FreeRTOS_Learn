#ifndef NB_MODE_LIB_H
#define NB_MODE_LIB_H

#include "fm33lc0xx_fl.h"
#include "Define.h"
#include "PublicLib_No_One.h"

#define NB_PWR_HIGH             GPIO_SET_H(GPIOC,FL_GPIO_PIN_5)
#define NB_PWR_LOW              GPIO_SET_L(GPIOC,FL_GPIO_PIN_5)
extern void NB_Mode_Init(void);
extern int daysFormOneMonthOneDay(int year, int month, int day);
extern char readNowSendMode(int DayNumFormStartDay);    // 读取当前发送模式
extern uint32_t InitSendModeAndTimeTask(void);

#endif

