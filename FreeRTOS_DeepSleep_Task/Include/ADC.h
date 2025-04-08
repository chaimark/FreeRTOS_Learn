#ifndef ADC_H
#define ADC_H

#include "PublicLib_No_One.h"

extern void Test_BatVoltge(void);
#ifdef IsTemperature
extern void Test_InsideT(void);
#else
extern void Get_Press_Value(void)
#endif

#endif
