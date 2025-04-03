#ifndef __DEFINE_H
#define __DEFINE_H
// #include "LPUart.h"
// #include "Uart1.h"
#include "../MF-config/Inc/mf_config.h"
#include "AT24C02.h"

extern const char SOFT_VERSION; // 版本号
extern unsigned int UART_BAND;

#define LONG_UARTMAX   1024
typedef struct {
    char TxBuf[LONG_UARTMAX];
    uint16_t TxLen;
    char TxOpc;
    char RxBuf[LONG_UARTMAX];
    uint16_t RxLen;
    char RxOpc;
    char Rxdata;
}LongUART;

#define UARTMAX   255
typedef struct {
    char TxBuf[UARTMAX];
    uint16_t TxLen;
    char TxOpc;
    char RxBuf[UARTMAX];
    uint16_t RxLen;
    char RxOpc;
    char Rxdata;
}ShortUART;

extern LongUART UART1Ddata;
extern ShortUART LPUART0Ddata;

extern unsigned int Now_DEV_Volt;
extern unsigned int MENU_Id;

#define INSIDET_PWR_HIGH        FL_GPIO_SetOutputPin(GPIOD,FL_GPIO_PIN_5)
#define INSIDET_PWR_LOW         FL_GPIO_ResetOutputPin(GPIOD,FL_GPIO_PIN_5)

#define EEprom_SDA_HIGH         FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_12)
#define EEprom_SDA_LOW          FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_12)  
#define EEprom_SDA_VALUE        (FL_GPIO_ReadInputPort(GPIOA)&FL_GPIO_PIN_12)
#define EEprom_SCL_OUT          FL_GPIO_SetPinMode(GPIOA, FL_GPIO_PIN_11,FL_GPIO_MODE_OUTPUT)
#define EEprom_SCL_LOW          FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_11) 
#define EEprom_SCL_HIGH         FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_11) 

#define EEPROM_AT24CXXDATA_ADDRESS 0    // AT24CxxData
#define EEPROM_OTHER_ADDRESS 184       // EEPROM_Other

#include "gpio.h"
#include "BSTim.h"
#include "LPTim32.h"
#include "../Interflow/RTC.h"
#define NowYear RTC_Date.year
#define NowMonth RTC_Date.month
#define NowDay RTC_Date.day
#define NowWeek RTC_Date.week
#define NowHour RTC_Date.hour
#define NowMinute RTC_Date.minute
#define NowSecond RTC_Date.second

#define GetDateHex(DateIemt) anyBaseToAnyBase(DateIemt, 10, 16) // 获取当前年月日的16进制值
#include "../Interflow/StrLib.h"


extern void Device_Init(void);
extern void StartOPenDevMode(void);
extern void CheckMeterNum(void);


#endif

