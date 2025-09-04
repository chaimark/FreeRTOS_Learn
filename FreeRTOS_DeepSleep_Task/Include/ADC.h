#ifndef ADC_H
#define ADC_H

#include "PublicLib_No_One.h"
#include "Define.h"
#define VOL_GND_HIGH                    GPIO_SET_H(GPIOB, FL_GPIO_PIN_1)
#define VOL_GND_LOW                     GPIO_SET_L(GPIOB, FL_GPIO_PIN_1)
#define Read_External_PWR               (FL_GPIO_ReadInputPort(GPIOD)&FL_GPIO_PIN_2)

extern void MF_ADC_Common_Init(void);
extern void MF_ADC_Sampling_Init(bool isCheckTemper);

extern void Test_BatVoltge(void);
extern void Test_Temperature(void);
extern void Test_ValueDegree(int8_t isSaveR_10K, bool isSetEquTest);

#endif
