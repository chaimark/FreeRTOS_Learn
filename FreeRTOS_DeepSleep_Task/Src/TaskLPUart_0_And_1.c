#include "TaskLPUart_0_And_1.h"
#include "LPUart_0_And_1_Lib.h"
#include "RTC_SetTime.h"
#include "AT24CXXDataLoader.h"
#include "PublicLib_No_One.h"
#include "StrLib.h"

uint8_t getUartx = 0xFF;
void OpenBSTime(void) {
    if (getUartx == 0xFF) {
        BSTIM32_Setup();    //开启定时器中断
        getUartx = 0xFE;
    }
}
TaskHandle_t LPUart_0_And_1_Hand = NULL;
SemaphoreHandle_t LPUartxSemaphore = NULL;
void LPUart0CheckBuff(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // 向 TimeTask 发出信号量, 准备继续运行
    xSemaphoreGiveFromISR(LPUartxSemaphore, &xHigherPriorityTaskWoken);
    // 恢复 TimeTask 任务
    if (xTaskResumeFromISR(LPUart_0_And_1_Hand) == pdFALSE) {
        // 如果需要切换上下文
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    getUartx = 0;
}
void LPUart1CheckBuff(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // 向 TimeTask 发出信号量, 准备继续运行
    xSemaphoreGiveFromISR(LPUartxSemaphore, &xHigherPriorityTaskWoken);
    // 恢复 TimeTask 任务
    if (xTaskResumeFromISR(LPUart_0_And_1_Hand) == pdFALSE) {
        // 如果需要切换上下文
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    getUartx = 1;
}
void LPUart_0_And_1_Receive(void * pvParameters) {
    getUartx = 0xFF;
    LPUartxSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    while (1) {
        if (xSemaphoreTake(LPUartxSemaphore, portMAX_DELAY) == pdTRUE) {
            if (getUartx == 0) {
                if (HY_USB_TTL_CheckBuff(LPUART0Ddata.RxBuf, LPUART0Ddata.RxLen, getUartx)) {
                    vTaskDelay(500);
                    LPUART0_Send((unsigned char *)LPUART0Ddata.TxBuf, LPUART0Ddata.TxLen);
                }
                Clr_LPUart0_RxBuffer();
            }
            if (getUartx == 1) {
                if (HY_USB_TTL_CheckBuff(LPUART1Ddata.RxBuf, LPUART1Ddata.RxLen, getUartx)) {
                    vTaskDelay(500);
                    LPUART1_Send((unsigned char *)LPUART1Ddata.TxBuf, LPUART1Ddata.TxLen);
                }
                Clr_LPUart1_RxBuffer();
            }
            getUartx = 0xFF;
            BSTIM32_Stop();     //关闭定时器中断
        }
    }
}


