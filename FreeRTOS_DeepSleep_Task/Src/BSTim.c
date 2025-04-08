#include "BSTim.h"
#include "fm33lc0xx_fl.h"
#include "SetTime.h"

// 5ms
void MF_BSTIM32_Init(void) {
    FL_BSTIM32_InitTypeDef bstim32;                          // 定义一个结构体变量用于配置BSTIM32定时器
    FL_NVIC_ConfigTypeDef nvic;                              // 定义一个结构体变量用于配置NVIC（嵌套向量中断控制器）
    bstim32.prescaler = 640 - 1;                             // 设置定时器预分频器, 640-1表示预分频系数为640
    bstim32.autoReload = 500 - 1;                           // 设置自动重装载值
    bstim32.autoReloadState = FL_ENABLE;                     // 启用自动重装载功能
    bstim32.clockSource = FL_RCC_BSTIM32_CLK_SOURCE_APB2CLK; // 设置定时器时钟源为APB2时钟
    FL_BSTIM32_Init(BSTIM32, &bstim32);                      // 初始化BSTIM32定时器, 使用上面配置的结构体变量
    FL_BSTIM32_ClearFlag_Update(BSTIM32);                    // 清除BSTIM32定时器的更新标志位
    FL_BSTIM32_EnableIT_Update(BSTIM32);                     // 启用BSTIM32定时器的更新中断
    FL_BSTIM32_Enable(BSTIM32);                              // 启用BSTIM32定时器
    nvic.preemptPriority = 0x0002;                             // 设置NVIC的中断优先级为1
    FL_NVIC_Init(&nvic, BSTIM_IRQn);                         // 初始化NVIC, 使用上面配置的结构体变量
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
        CountSetTimeTask();
    }
}
