#include "PowerCtrl.h"
#include "TaskLPUart_0_And_1.h"
#include "RTC_SetTime.h"
#include "SetTime.h"
#include "BSTim.h"
#include "SX1276.h"
#include "LPUart_0_And_1_Lib.h"

void EnterLowPowerMode(void * InputConfig) {
    FL_IWDT_ReloadCounter(IWDT);    // 8 分钟之内 RTC 帮忙喂狗
    static uint8_t isSendTaskA = 0;
    isSendTaskA--;
    if (isSendTaskA == 0)
        // LPUART1_Send((unsigned char *)"AAA\r\n", strlen("AAA\r\n")); // 发送测试数据
    return;
}
void ExitLowPowerMode(void * InputConfig) {
    FL_IWDT_ReloadCounter(IWDT);    // 8 分钟之内 RTC 帮忙喂狗
    static uint8_t isSendTaskB = 0;
    isSendTaskB--;
    if (isSendTaskB == 0)
        // LPUART1_Send((unsigned char *)"BBB\r\n", strlen("BBB\r\n")); // 发送测试数据
    return;
}




