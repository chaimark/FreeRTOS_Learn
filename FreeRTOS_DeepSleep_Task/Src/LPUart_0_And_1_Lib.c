#include <string.h>
#include "Define.h"
#include "BSTim.h"
#include "LPUart_0_And_1_Lib.h"
#include "TaskLPUart_0_And_1.h"

void LPUART_Init(LPUART_Type * LPUARTx) {
    FL_LPUART_InitTypeDef  InitStructer = {0};
    FL_GPIO_InitTypeDef  GPIO_InitStruct = {0};
    FL_NVIC_ConfigTypeDef InterruptConfigStruct = {0};
    volatile unsigned char tmp08;

    if (LPUARTx == LPUART0) {
        GPIO_InitStruct.pin = FL_GPIO_PIN_13;
        GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;              //数字功能
        GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;     //推挽输出
        GPIO_InitStruct.pull = FL_ENABLE;                            //接收使能上拉
        GPIO_InitStruct.remapPin = FL_ENABLE;                        //使能额外数字功能
        FL_GPIO_Init(GPIOA, &GPIO_InitStruct);                     //PA13

        GPIO_InitStruct.pin = FL_GPIO_PIN_14;
        GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;               //数字功能
        GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;      //推挽输出
        GPIO_InitStruct.pull = FL_DISABLE;                            //发送不上拉
        GPIO_InitStruct.remapPin = FL_ENABLE;                         //使能额外数字功能    
        FL_GPIO_Init(GPIOA, &GPIO_InitStruct);                      //PA14

        InterruptConfigStruct.preemptPriority = 0x0001;
        FL_NVIC_Init(&InterruptConfigStruct, LPUART0_IRQn);
    }
    if (LPUARTx == LPUART1) {
        GPIO_InitStruct.pin = FL_GPIO_PIN_2;
        GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;                //数字功能
        GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;       //推挽输出
        GPIO_InitStruct.pull = FL_ENABLE;                              //接收使能上拉
        GPIO_InitStruct.remapPin = FL_ENABLE;                          //使能额外数字功能  
        FL_GPIO_Init(GPIOC, &GPIO_InitStruct);                       //PC2

        GPIO_InitStruct.pin = FL_GPIO_PIN_3;
        GPIO_InitStruct.mode = FL_GPIO_MODE_DIGITAL;                //数字功能
        GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;       //推挽输出
        GPIO_InitStruct.pull = FL_DISABLE;                             //发送不上拉
        GPIO_InitStruct.remapPin = FL_ENABLE;                          //使能额外数字功能  
        FL_GPIO_Init(GPIOC, &GPIO_InitStruct);                       //PC3

        InterruptConfigStruct.preemptPriority = 0x0001;
        FL_NVIC_Init(&InterruptConfigStruct, LPUART1_IRQn);
    }
    InitStructer.clockSrc = FL_RCC_LPUART_CLK_SOURCE_LSCLK;  //时钟源LSCLK
    InitStructer.baudRate = FL_LPUART_BAUDRATE_9600;  //波特率9600
    InitStructer.dataWidth = FL_LPUART_DATA_WIDTH_8B;   //8位数据位
    InitStructer.stopBits = FL_LPUART_STOP_BIT_WIDTH_1B;     //1位停止位
    InitStructer.parity = FL_LPUART_PARITY_NONE;   //无校验
    InitStructer.transferDirection = FL_LPUART_DIRECTION_TX_RX;   //使能发送和接收
    FL_LPUART_Init(LPUARTx, &InitStructer);
}

void MF_LPUART0_Interrupt_Init(void) {
    FL_LPUART_ClearFlag_RXBuffFull(LPUART0);
    FL_LPUART_EnableIT_RXBuffFull(LPUART0);
}
void MF_LPUART1_Interrupt_Init(void) {
    FL_LPUART_ClearFlag_RXBuffFull(LPUART1);
    FL_LPUART_EnableIT_RXBuffFull(LPUART1);
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
        //中断转发接收到的数据
        LPUART0Ddata.Rxdata = FL_LPUART_ReadRXBuff(LPUART0);//接收中断标志可通过读取rxreg寄存器清除
        if (LPUART0Ddata.RxLen >= UARTMAX) { // 超过最大 LONG_LPUARTMAX
            Clr_LPUart0_RxBuffer();
            return;
        } else {
            OpenBSTime();
            Save_LPUART0_RX_Data(LPUART0Ddata.Rxdata);
        }
        // 启动一个 30ms 定时器, 30ms 后获取buff中的数据（如果有新中断, 则复位计时器）
        SetTime.InitSetTimeTask(RecTimeLPUART0, SecTo10Ms(0.030), LPUART0_ReadData_Deal); // 30ms
    }
}

//======================================================================================================================//
void LPUART1_Send(unsigned char * txBuffer, int length) {
    if (length == 0) {
        FL_LPUART_ClearFlag_TXShiftBuffEmpty(LPUART1);  //清除发送中断标志
        return;
    }
    for (int i = 0; i < length; i++) {
        while (!FL_LPUART_IsActiveFlag_TXBuffEmpty(LPUART1));
        FL_LPUART_WriteTXBuff(LPUART1, txBuffer[i]);
    }
    while (!FL_LPUART_IsActiveFlag_TXShiftBuffEmpty(LPUART1));
    FL_LPUART_ClearFlag_TXShiftBuffEmpty(LPUART1);  //清除发送中断标志
}
//===========================================================//
void Clr_LPUart1_RxBuffer(void) {
    memset(LPUART1Ddata.RxBuf, 0x00, LPUART1Ddata.RxLen);
    LPUART1Ddata.RxLen = 0;
}
//===========================================================//
void Save_LPUART1_RX_Data(unsigned char buf) {
    LPUART1Ddata.RxBuf[LPUART1Ddata.RxLen++] = buf;
}
//===========================================================//
void LPUART1_ReadData_Deal(void) {
    SetTime.CloseTask(RecTimeLPUART1);
    if (LPUART1Ddata.RxLen > 0) { // 超过 4
        LPUart1CheckBuff();
    }
}
//===========================================================//
void LPUART1_IRQHandler(void) {
    //接收中断处理
    if ((FL_ENABLE == FL_LPUART_IsEnabledIT_RXBuffFull(LPUART1)) && (FL_SET == FL_LPUART_IsActiveFlag_RXBuffFull(LPUART1))) {
        //中断转发接收到的数据
        LPUART1Ddata.Rxdata = FL_LPUART_ReadRXBuff(LPUART1);//接收中断标志可通过读取rxreg寄存器清除
        if (LPUART1Ddata.RxLen >= UARTMAX) { // 超过最大 LONG_LPUARTMAX
            Clr_LPUart1_RxBuffer();
            return;
        } else {
            OpenBSTime();
            Save_LPUART1_RX_Data(LPUART1Ddata.Rxdata);
        }
        // 启动一个 30ms 定时器, 30ms 后获取buff中的数据（如果有新中断, 则复位计时器）
        SetTime.InitSetTimeTask(RecTimeLPUART1, SecTo10Ms(0.030), LPUART1_ReadData_Deal); // 30ms
    }
}

