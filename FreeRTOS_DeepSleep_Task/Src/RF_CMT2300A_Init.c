#include "Define.h"
#define TURE                1
#define FALSE               0

#include "CMT2300A.h"

void CMT2300A_PWR_ON(void) {
    CMT2300A_PWR_Out();
    CMT2300A_PWR_HIGH;
    FL_DelayMs(200);
}

void CMT2300A_PWR_OFF(void) {
    CMT2300A_PWR_Out();
    CMT2300A_PWR_LOW;
    FL_DelayMs(50);
    CMT_SPI_SCLK_Out();
    CMT_SPI_CSB_Out();
    CMT_SPI_SCLK_L();
    CMT_SPI_CSB_L();
    CMT_SPI_SDIO_Set_Out();
    CMT_SPI_SDIO_L();
    CMT_SPI_GPIO1_Set_In();
    CMT_SPI_FCSB_Out();
    CMT_SPI_FCSB_L();
}

void CMT2300A_RF_OPEN(void) {
    SetErrerCode(CODE_ERR_WIRELESS_FAULT, true);
    unsigned char RFFreq_china = 0x00;
    unsigned int RFFreq_chinaTemp = 0;
    CMT2300A_PWR_ON();
    FL_DelayMs(200);
    RF_Init();

    if (FALSE == CMT2300A_IsExist()) {  //CMT2300A not found
        FL_DelayMs(10);
    } else {             //CMT2300A ready
        FL_DelayMs(10);
    }
    RFFreq_china = METERID[3];
    if (RFFreq_china == 0x00) {
        RFFreq_chinaTemp = ((METERID[3] & 0xF0) >> 4) * 10 + (METERID[3] & 0x0F);
    }
    if ((RFFreq_china > 0x00) && (RFFreq_china <= 0x20)) {
        RFFreq_chinaTemp = ((METERID[3] & 0xF0) >> 4) * 10 + (METERID[3] & 0x0F);
    }
    if ((RFFreq_china > 0x20) && (RFFreq_china <= 0x40)) {
        RFFreq_chinaTemp = ((METERID[3] & 0xF0) >> 4) * 10 + (METERID[3] & 0x0F) - 20;
    }
    if ((RFFreq_china > 0x40) && (RFFreq_china <= 0x60)) {
        RFFreq_chinaTemp = ((METERID[3] & 0xF0) >> 4) * 10 + (METERID[3] & 0x0F) - 40;
    }
    if ((RFFreq_china > 0x60) && (RFFreq_china <= 0x80)) {
        RFFreq_chinaTemp = ((METERID[3] & 0xF0) >> 4) * 10 + (METERID[3] & 0x0F) - 60;
    }
    if ((RFFreq_china > 0x80) && (RFFreq_china <= 0x99)) {
        RFFreq_chinaTemp = ((METERID[3] & 0xF0) >> 4) * 10 + (METERID[3] & 0x0F) - 80;
    }
    switch (RFFreq_chinaTemp) {
        case 0:
            CMT2300A_PollRx(0);
            break;
        case 1:
            CMT2300A_PollRx(4);
            break;
        case 2:
            CMT2300A_PollRx(8);
            break;
        case 3:
            CMT2300A_PollRx(12);
            break;
        case 4:
            CMT2300A_PollRx(16);
            break;
        case 5:
            CMT2300A_PollRx(20);
            break;
        case 6:
            CMT2300A_PollRx(24);
            break;
        case 7:
            CMT2300A_PollRx(28);
            break;
        case 8:
            CMT2300A_PollRx(32);
            break;
        case 9:
            CMT2300A_PollRx(36);
            break;
        case 10:
            CMT2300A_PollRx(40);
            break;
        case 11:
            CMT2300A_PollRx(44);
            break;
        case 12:
            CMT2300A_PollRx(48);
            break;
        case 13:
            CMT2300A_PollRx(52);
            break;
        case 14:
            CMT2300A_PollRx(56);
            break;
        case 15:
            CMT2300A_PollRx(60);
            break;
        case 16:
            CMT2300A_PollRx(64);
            break;
        case 17:
            CMT2300A_PollRx(68);
            break;
        case 18:
            CMT2300A_PollRx(72);
            break;
        case 19:
            CMT2300A_PollRx(76);
            break;
        case 20:
            CMT2300A_PollRx(80);
            break;
        default:
            CMT2300A_PollRx(0);
            break;
    }
    SetErrerCode(CODE_ERR_WIRELESS_FAULT, false);
}

void CMT2300A_RF_CLOSE() {
    CMT2300A_PWR_OFF();
    FL_DelayMs(600);
}

void CMT2300A_GPIO_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* PC8 Read 433 Exterior Power */
    GPIO_InitStruct.pin = FL_GPIO_PIN_8;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;// FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    // PC11_CMT2800A_PWR 
    GPIO_InitStruct.pin = FL_GPIO_PIN_11;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_SET_H(GPIOC, FL_GPIO_PIN_11);
    // PD6_SDIO
    GPIO_InitStruct.pin = FL_GPIO_PIN_6;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    // PD5_GPIO1    PD4_CSB    PD3_SCLK    PD1_FCSB
    GPIO_InitStruct.pin = FL_GPIO_PIN_5 | FL_GPIO_PIN_4 | FL_GPIO_PIN_3 | FL_GPIO_PIN_1;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    GPIO_SET_L(GPIOD, FL_GPIO_PIN_5 | FL_GPIO_PIN_4 | FL_GPIO_PIN_3);
    GPIO_SET_H(GPIOD, FL_GPIO_PIN_1);

    FL_EXTI_InitTypeDef     EXTI_InitStruct;
    FL_NVIC_ConfigTypeDef   InterruptConfigStruct;
    /***********************GD0**********************************************/
    /* PD0 EXTI10_Select3 */
    GPIO_InitStruct.pin = FL_GPIO_PIN_0;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;// FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    EXTI_InitStruct.input = FL_GPIO_EXTI_INPUT_GROUP3;
    EXTI_InitStruct.triggerEdge = FL_GPIO_EXTI_TRIGGER_EDGE_FALLING;
    EXTI_InitStruct.filter = FL_ENABLE;
    FL_EXTI_Init(FL_GPIO_EXTI_LINE_12, &EXTI_InitStruct);
    FL_GPIO_ClearFlag_EXTI(GPIO, FL_GPIO_EXTI_LINE_12);
    /***********************end GD0**********************************************/
    InterruptConfigStruct.preemptPriority = 0x0001;
    FL_NVIC_Init(&InterruptConfigStruct, GPIO_IRQn);
}

