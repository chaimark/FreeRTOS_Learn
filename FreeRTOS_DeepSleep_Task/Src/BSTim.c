#include "BSTim.h"
#include "fm33lc0xx_fl.h"
#include "SetTime.h"

// 10ms
void MF_BSTIM32_Init(void) {
    FL_BSTIM32_InitTypeDef bstim32;                             // 定义一个结构体变量用于配置BSTIM32定时器
    FL_NVIC_ConfigTypeDef InterruptConfigStruct;                // 定义一个结构体变量用于配置NVIC（嵌套向量中断控制器）
    bstim32.prescaler = 0;                                      // 设置定时器预分频器, 1表示预分频系数为1
    bstim32.autoReload = 327 - 1;                               // 设置自动重装载值
    bstim32.autoReloadState = FL_ENABLE;                        // 启用自动重装载功能
    bstim32.clockSource = FL_RCC_BSTIM32_CLK_SOURCE_LSCLK;
    FL_BSTIM32_Init(BSTIM32, &bstim32);                         // 初始化BSTIM32定时器, 使用上面配置的结构体变量
    FL_BSTIM32_ClearFlag_Update(BSTIM32);                       // 清除BSTIM32定时器的更新标志位
    FL_BSTIM32_EnableIT_Update(BSTIM32);                        // 启用BSTIM32定时器的更新中断
    InterruptConfigStruct.preemptPriority = 0x000C;             // 设置NVIC的中断优先级为2
    FL_NVIC_Init(&InterruptConfigStruct, BSTIM_IRQn);           // 初始化NVIC, 使用上面配置的结构体变量
    FL_BSTIM32_Enable(BSTIM32);                                 // 启用BSTIM32定时器
}

void BSTIM32_Setup(void) {
    MF_BSTIM32_Init();
    /* Enable BSTIM32 */
    FL_BSTIM32_Enable(BSTIM32);
}

void BSTIM32_Stop(void) {
    /* Enable BSTIM32 */
    FL_BSTIM32_Disable(BSTIM32);
}

void BSTIM_IRQHandler(void) {
    if (FL_BSTIM32_IsActiveFlag_Update(BSTIM32)) {
        FL_BSTIM32_ClearFlag_Update(BSTIM32);
#ifdef BSTIM_OPEN_AND_TASK_NUM
        CountSetTimeTask();
#endif
    }
}
