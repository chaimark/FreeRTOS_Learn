#include "fm33lc0xx_fl.h"
#include "Define.h"
#include "Main.h"
#include "LPTim32.h"

void MF_LPTIM_Init(void) {
    FL_LPTIM32_InitTypeDef lptim32;
    FL_NVIC_ConfigTypeDef    InterruptConfigStruct;
    lptim32.clockSource = FL_RCC_LPTIM32_CLK_SOURCE_LSCLK;
    lptim32.prescalerClockSource = FL_LPTIM32_CLK_SOURCE_INTERNAL;
    lptim32.prescaler = FL_LPTIM32_PSC_DIV1;
    lptim32.autoReload = 32768;  // 32.768*1000 - 1
    lptim32.mode = FL_LPTIM32_OPERATION_MODE_NORMAL;
    lptim32.onePulseMode = FL_LPTIM32_ONE_PULSE_MODE_CONTINUOUS;
    lptim32.countEdge = FL_LPTIM32_ETR_COUNT_EDGE_RISING;
    lptim32.triggerEdge = FL_LPTIM32_ETR_TRIGGER_EDGE_RISING;
    FL_LPTIM32_Init(LPTIM32, &lptim32);
    FL_LPTIM32_ClearFlag_Update(LPTIM32);
    FL_LPTIM32_EnableIT_Update(LPTIM32);
    InterruptConfigStruct.preemptPriority = 0x0002;
    FL_NVIC_Init(&InterruptConfigStruct, LPTIM_IRQn);
    FL_LPTIM32_Enable(LPTIM32);
}

void LPTIM_IRQHandler(void) {
    if (FL_LPTIM32_IsActiveFlag_Update(LPTIM32)) {
        FL_LPTIM32_ClearFlag_Update(LPTIM32);
#ifdef LPTIM_OPEN_AND_TASK_NUM
        LPCountSetTimeTask();
#endif
    }
}

void LPTIM_Setup(void) {
    MF_LPTIM_Init();
    /* Enable BSTIM32 */
    FL_LPTIM32_Enable(LPTIM32);
}

void LPTIM_Stop(void) {
    /* Enable BSTIM32 */
    FL_LPTIM32_Disable(LPTIM32);
}

