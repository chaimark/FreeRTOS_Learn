#include "lptim32_timing.h"

void LPTIM32_Init(void)
{
    FL_LPTIM32_InitTypeDef  initStruct_LPTIM32={0};
    FL_LPTIM32_OC_InitTypeDef initStruct_OC = {0};
    initStruct_LPTIM32.clockSource           = FL_RCC_LPTIM32_CLK_SOURCE_LSCLK;
    initStruct_LPTIM32.prescalerClockSource  = FL_LPTIM32_CLK_SOURCE_INTERNAL;
    initStruct_LPTIM32.prescaler             = FL_LPTIM32_PSC_DIV32;
    initStruct_LPTIM32.autoReload            = 0xFFFFFFFF;
    initStruct_LPTIM32.mode                  = FL_LPTIM32_OPERATION_MODE_NORMAL;
    initStruct_LPTIM32.countEdge             = FL_LPTIM32_ETR_COUNT_EDGE_RISING;
    initStruct_LPTIM32.triggerEdge           = FL_LPTIM32_ETR_TRIGGER_EDGE_RISING;
    initStruct_LPTIM32.onePulseMode          = FL_LPTIM32_ONE_PULSE_MODE_CONTINUOUS;
    
    FL_LPTIM32_Init(LPTIM32, &initStruct_LPTIM32);
    FL_LPTIM32_OC_Init(LPTIM32, FL_LPTIM32_CHANNEL_1, &initStruct_OC);
    FL_LPTIM32_ClearFlag_CC(LPTIM32,FL_LPTIM32_CHANNEL_1);
//    FL_LPTIM32_Enable(LPTIM32);   
}
void LPTIM_IRQHandler()
{
    FL_LPTIM32_ClearFlag_CC(LPTIM32,FL_LPTIM32_CHANNEL_1);
}
