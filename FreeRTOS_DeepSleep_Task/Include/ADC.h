#ifndef ADC_H
#define ADC_H

#include "PublicLib_No_One.h"

extern void MF_ADC_Common_Init(void);
extern void MF_ADC_Sampling_Init(bool isCheckTemper);

extern void Test_BatVoltge(void);
extern void Test_Temperature(void);
extern void Test_ValueDegree(void);

#endif
