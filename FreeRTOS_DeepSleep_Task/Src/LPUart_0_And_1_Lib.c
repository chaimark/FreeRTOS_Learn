#include <string.h>
#include "Define.h"
#include "BSTim.h"
#include "LPUart_0_And_1_Lib.h"
#include "TaskAllReceive.h"
#include "NB_Mode_Lib.h"

//////////////////////////////////////////////////////////////
SemaphoreHandle_t Uart1_RunFlag = NULL;
#define clearUart1RunFlag xSemaphoreGive(Uart1_RunFlag);
void isRunningForUart1(void) {
    // 获取互斥量，防止读写冲突
    while (!(xSemaphoreTake(Uart1_RunFlag, 100) == pdTRUE)) {
        vTaskDelay(100);
    }
}
void init_Uart1_RunFlag(void) {
    static bool isInit_RunFlag = false;
    if (isInit_RunFlag) {
        return;
    }
    Uart1_RunFlag = xSemaphoreCreateBinary(); // 初始化信号量
    xSemaphoreGive(Uart1_RunFlag);
}
//////////////////////////////////////////////////////////////
// 关闭红外串口
void CloseLPUart0TTL(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct;
    //PA13 LPUart0_RX 输出高电平
    GPIO_InitStruct.pin = FL_GPIO_PIN_13;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SET_L(GPIOA, FL_GPIO_PIN_13);

    //PA14 LPUart0_TX 输出低电平
    GPIO_InitStruct.pin = FL_GPIO_PIN_14;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SET_L(GPIOA, FL_GPIO_PIN_14);
    Infrared_POWER_OFF;
}
// 关闭 NB 串口
void CloseUart1TTL(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct;
    //PA13 LPUart0_RX 输出高电平
    GPIO_InitStruct.pin = FL_GPIO_PIN_13;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_SET_L(GPIOB, FL_GPIO_PIN_13);

    //PA14 LPUart0_TX 输出低电平
    GPIO_InitStruct.pin = FL_GPIO_PIN_14;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_SET_L(GPIOB, FL_GPIO_PIN_14);
    NB_PWR_LOW;
}
void LPUART0_Init(void) {
    FL_LPUART_InitTypeDef  InitStructer = {0};
    FL_GPIO_InitTypeDef  GPIO_InitStruct = {0};
    FL_NVIC_ConfigTypeDef InterruptConfigStruct = {0};

    GPIO_InitStruct.pin = FL_GPIO_PIN_15;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    Infrared_POWER_OFF;

    InitStructer.clockSrc = FL_RCC_LPUART_CLK_SOURCE_LSCLK;  //时钟源LSCLK
    InitStructer.transferDirection = FL_LPUART_DIRECTION_TX_RX;   //使能发送和接收
    GPIO_InitStruct.pin = FL_GPIO_PIN_13;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;               //数字功能
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;      //推挽输出
    GPIO_InitStruct.pull = FL_ENABLE;                          //接收使能上拉
    GPIO_InitStruct.remapPin = FL_ENABLE;                      //使能额外数字功能
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);                     //PA13
    GPIO_InitStruct.pin = FL_GPIO_PIN_14;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;               //数字功能
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;      //推挽输出
    GPIO_InitStruct.pull = FL_DISABLE;                         //发送不上拉
    GPIO_InitStruct.remapPin = FL_ENABLE;                      //使能额外数字功能    
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);                     //PA14
    InterruptConfigStruct.preemptPriority = 0x0001;
    FL_NVIC_Init(&InterruptConfigStruct, LPUART0_IRQn);

    InitStructer.baudRate = FL_LPUART_BAUDRATE_9600;        //波特率9600
    InitStructer.dataWidth = FL_LPUART_DATA_WIDTH_8B;       //8位数据位
    InitStructer.stopBits = FL_LPUART_STOP_BIT_WIDTH_1B;    //1位停止位
    InitStructer.parity = FL_LPUART_PARITY_NONE;            //无校验
    FL_LPUART_Init(LPUART0, &InitStructer);
}

void MF_UART1_Init(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct;
    FL_UART_InitTypeDef    UART1_InitStruct;
    FL_NVIC_ConfigTypeDef    InterruptConfigStruct;
    /* PB13 UART1_RX */
    GPIO_InitStruct.pin = FL_GPIO_PIN_13;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* PB14 UART1_TX */
    GPIO_InitStruct.pin = FL_GPIO_PIN_14;
    GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    UART1_InitStruct.clockSrc = FL_RCC_UART1_CLK_SOURCE_APB1CLK;
    UART1_InitStruct.baudRate = UART_BAND;
    UART1_InitStruct.dataWidth = FL_UART_DATA_WIDTH_8B;
    UART1_InitStruct.stopBits = FL_UART_STOP_BIT_WIDTH_1B;
    UART1_InitStruct.parity = FL_UART_PARITY_NONE;
    UART1_InitStruct.transferDirection = FL_UART_DIRECTION_TX_RX;
    FL_UART_Init(UART1, &UART1_InitStruct);

    InterruptConfigStruct.preemptPriority = 0x0001;
    FL_NVIC_Init(&InterruptConfigStruct, UART1_IRQn);
}

void MF_LPUART0_Interrupt_Init(void) {
    FL_LPUART_ClearFlag_RXBuffFull(LPUART0);
    FL_LPUART_EnableIT_RXBuffFull(LPUART0);
}
//======================================================================================================================//
void LPUART0_Send(unsigned char * txBuffer, int length) {
    if (length == 0) {
        FL_LPUART_ClearFlag_TXShiftBuffEmpty(LPUART0);  //清除发送中断标志
        return;
    }
    for (int i = 0; i < length; i++) {
        while (!FL_LPUART_IsActiveFlag_TXBuffEmpty(LPUART0));
        FL_LPUART_WriteTXBuff(LPUART0, txBuffer[i]);
    }
    while (!FL_LPUART_IsActiveFlag_TXShiftBuffEmpty(LPUART0));
    FL_LPUART_ClearFlag_TXShiftBuffEmpty(LPUART0);  //清除发送中断标志
}
//===========================================================//
void Clr_LPUart0_RxBuffer(void) {
    memset(LPUART0Ddata.RxBuf, 0x00, LPUART0Ddata.RxLen);
    LPUART0Ddata.RxLen = 0;
}
//===========================================================//
void Save_LPUART0_RX_Data(unsigned char buf) {
    LPUART0Ddata.RxBuf[LPUART0Ddata.RxLen++] = buf;
}
//===========================================================//
void LPUART0_ReadData_Deal(void) {
    SetTime.CloseTask(RecTimeLPUART0);
    if (LPUART0Ddata.RxLen > 0) { // 超过 4
        LPUart0CheckBuff();
    }
}
//===========================================================//
void LPUART0_IRQHandler(void) {
    //接收中断处理
    if ((FL_ENABLE == FL_LPUART_IsEnabledIT_RXBuffFull(LPUART0)) && (FL_SET == FL_LPUART_IsActiveFlag_RXBuffFull(LPUART0))) {
        // 启动一个 30ms 定时器, 30ms 后获取buff中的数据（如果有新中断, 则复位计时器）
        SetTime.InitSetTimeTask(RecTimeLPUART0, SecTo10Ms(0.030), LPUART0_ReadData_Deal);
        //中断转发接收到的数据
        LPUART0Ddata.Rxdata = FL_LPUART_ReadRXBuff(LPUART0);//接收中断标志可通过读取rxreg寄存器清除
        if (LPUART0Ddata.RxLen >= LONG_UARTMAX) { // 超过最大 LONG_LPUARTMAX
            Clr_LPUart0_RxBuffer();
            return;
        } else {
            OpenBSTime(true);
            Save_LPUART0_RX_Data(LPUART0Ddata.Rxdata);
        }
    }
}

void MF_UART1_Interrupt_Init(void) {
    FL_UART_ClearFlag_RXBuffFull(UART1);
    FL_UART_EnableIT_RXBuffFull(UART1);
}
//======================================================================================================================//
void UART1_Send(unsigned char * txBuffer, int length) {
    if (length == 0) {
        FL_UART_ClearFlag_TXShiftBuffEmpty(UART1);  //清除发送中断标志
        return;
    }
    isRunningForUart1();
    for (int i = 0; i < length; i++) {
        while (!FL_UART_IsActiveFlag_TXBuffEmpty(UART1));
        FL_UART_WriteTXBuff(UART1, txBuffer[i]);
    }
    while (!FL_UART_IsActiveFlag_TXShiftBuffEmpty(UART1));
    FL_UART_ClearFlag_TXShiftBuffEmpty(UART1);  //清除发送中断标志
    clearUart1RunFlag;
}
//===========================================================//
void Clr_Uart1_RxBuffer(void) {
    memset(UART1Ddata.RxBuf, 0x00, UART1Ddata.RxLen);
    UART1Ddata.RxLen = 0;
}
//===========================================================//
void Save_UART1_RX_Data(unsigned char buf) {
    UART1Ddata.RxBuf[UART1Ddata.RxLen++] = buf;
}
//===========================================================//
void UART1_ReadData_Deal(void) {
    SetTime.CloseTask(RecTimeUART1);
    if (UART1Ddata.RxLen > 0) { // 超过 4
        NBUartCheckBuff();
    }
}
//===========================================================//
void UART1_IRQHandler(void) {
    //接收中断处理
    if ((FL_ENABLE == FL_UART_IsEnabledIT_RXBuffFull(UART1)) && (FL_SET == FL_UART_IsActiveFlag_RXBuffFull(UART1))) {
        // 启动一个 30ms 定时器, 30ms 后获取buff中的数据（如果有新中断, 则复位计时器）
        SetTime.InitSetTimeTask(RecTimeUART1, SecTo10Ms(0.030), UART1_ReadData_Deal);
        //中断转发接收到的数据
        UART1Ddata.Rxdata = FL_UART_ReadRXBuff(UART1);//接收中断标志可通过读取rxreg寄存器清除
        if (UART1Ddata.RxLen >= UARTMAX) { // 超过最大 LONG_UARTMAX
            Clr_Uart1_RxBuffer();
            return;
        } else {
            OpenBSTime(true);
            Save_UART1_RX_Data(UART1Ddata.Rxdata);
        }
    }
}

