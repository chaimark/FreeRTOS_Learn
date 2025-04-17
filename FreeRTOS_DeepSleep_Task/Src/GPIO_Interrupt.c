#include "Define.h"
#include "fm33lc0xx.h"
#include "fm33lc0xx_fl_gpio.h"

extern void SX1276_Interupt(void);
void GPIO_IRQHandler(void) {
    if (FL_GPIO_IsActiveFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_4)) {//S3
        FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_4);
        SX1276_Interupt();
    }
}

