#ifndef __ADC_H__
#define __ADC_H__

#include "fm33lc0xx_fl.h"

#define ADC_VREF    (*((uint16_t *)(0x1FFFFB08)))   // 30℃ vref1.22采样值
#define ADC_TS       (*((uint16_t *)(0x1FFFFa92)))   // 30℃ PTAT采样值	
extern uint32_t GetSingleChannelVoltage_POLL(uint32_t channel);
extern void ADC_Init(void);
#endif 
