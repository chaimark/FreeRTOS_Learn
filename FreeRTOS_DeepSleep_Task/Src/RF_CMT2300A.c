/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, CMOSTEK SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (C) CMOSTEK SZ.
 */

/*!
 * @file    radio.c
 * @brief   Generic radio handlers
 *
 * @version 1.2
 * @date    Jul 17 2017
 * @author  CMOSTEK R@D
 */

//#include <string.h>
#include "Define.h"
#include "RF_CMT2300A.h"
#include "CMT2300A_Params.h"

#define TURE                1
#define FALSE               0

#define RF_PACKET_SIZE 32               /* Define the payload size here */

unsigned char CMT2300A_RF_TX[20];
unsigned char CMT2300A_RF_RX[40];
#include "CMT2300A_SPI.h"
#include "RF_CMT2300A_Receive.h"

void RF_Init(void) {
    unsigned char tmp;
    CMT2300A_InitGpio();
    CMT2300A_Init();
    /* Config registers */
    CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR, g_cmt2300aCmtBank, CMT2300A_CMT_BANK_SIZE);
    CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR, g_cmt2300aSystemBank, CMT2300A_SYSTEM_BANK_SIZE);
    CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR, g_cmt2300aFrequencyBank, CMT2300A_FREQUENCY_BANK_SIZE);
    CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR, g_cmt2300aDataRateBank, CMT2300A_DATA_RATE_BANK_SIZE);
    CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR, g_cmt2300aBasebandBank, CMT2300A_BASEBAND_BANK_SIZE);
    CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR, g_cmt2300aTxBank, CMT2300A_TX_BANK_SIZE);
    // xosc_aac_code[2:0] = 2
    tmp = (~0x07) & CMT2300A_ReadReg(CMT2300A_CUS_CMT10);
    CMT2300A_WriteReg(CMT2300A_CUS_CMT10, tmp | 0x02);
    RF_Config();
}

void CMT2300A_PollRx(unsigned char chanel) {
    CMT2300A_GoStby();
    CMT2300A_SetAfcOvfTh(10);
    CMT2300A_SetFrequencyChannel(chanel);
    /* Must clear FIFO after enable SPI to read or write the FIFO */
    CMT2300A_EnableReadFifo();
    CMT2300A_ClearInterruptFlags();
    CMT2300A_ClearRxFifo();
    CMT2300A_GoRx();
}

void RF_Config(void) {
#ifdef ENABLE_ANTENNA_SWITCH
  /* If you enable antenna switch, GPIO1/GPIO2 will output RX_ACTIVE/TX_ACTIVE,
     and it can't output INT1/INT2 via GPIO1/GPIO2 */
    CMT2300A_EnableAntennaSwitch(0);
#else
  /* Config GPIOs */
    CMT2300A_ConfigGpio(
        CMT2300A_GPIO1_SEL_INT1 | /* INT1 > GPIO1 */
        CMT2300A_GPIO2_SEL_INT2 | /* INT2 > GPIO2 */
        CMT2300A_GPIO3_SEL_DOUT
    );
    /* Config interrupt */
    CMT2300A_ConfigInterrupt(
        CMT2300A_INT_SEL_RSSI_VLD, /* Config INT1  PJD �ж�*/
        CMT2300A_INT_SEL_PKT_OK   /* Config INT2 */
    );
#endif
  /* Enable interrupt */
    CMT2300A_EnableInterrupt(
        CMT2300A_MASK_TX_DONE_EN |
        CMT2300A_MASK_PREAM_OK_EN |
        CMT2300A_MASK_SYNC_OK_EN |
        CMT2300A_MASK_NODE_OK_EN |
        CMT2300A_MASK_CRC_OK_EN |
        CMT2300A_MASK_PKT_DONE_EN
    );
    /* Disable low frequency OSC calibration */
    CMT2300A_EnableLfosc(FALSE);
    /* Use a single 64-byte FIFO for either Tx or Rx */
    //CMT2300A_EnableFifoMerge(TRUE);  
    //CMT2300A_SetFifoThreshold(16);
    /* This is optional, only needed when using Rx fast frequency hopping */
    /* See AN142 and AN197 for details */
    //CMT2300A_SetAfcOvfTh(0x27);  
    /* Go to sleep for configuration to take effect */
    CMT2300A_GoSleep();
}

void CMT2300A_RX_Interrupt(void) {
    FL_DelayMs(100);
    CMT2300A_GoStby();
    CMT2300A_ReadFifo(&CMT2300A_T_temp.RF_RxLen, 1);
    CMT2300A_ReadFifo(CMT2300A_T_temp.RFRxBuf, CMT2300A_T_temp.RF_RxLen);
    CMT2300A_ClearRxFifo();
    CMT2300A_ClearInterruptFlags();
    CMT2300A_GoRx();
    CMT2300A_RECEIVE_DATA();
}

unsigned char Radio_Send_FixedLen(const unsigned char pBuf[], unsigned char len) {
    unsigned long delay;
    CMT2300A_GoStby();
    CMT2300A_ClearInterruptFlags();
    CMT2300A_ClearTxFifo();
    CMT2300A_EnableWriteFifo();
    CMT2300A_SetPayloadLength(len);
    CMT2300A_WriteFifo(pBuf, len); //TX_FIFO
    CMT2300A_GoTx();
    delay = 400;
    while (1) {
        if (CMT2300A_MASK_TX_DONE_FLG & CMT2300A_ReadReg(CMT2300A_CUS_INT_CLR1)) {  /* Read TX_DONE flag */
            FL_DelayMs(1);
            CMT2300A_GoStby();
            CMT2300A_EnableReadFifo();
            CMT2300A_ClearInterruptFlags();
            CMT2300A_ClearRxFifo();
            CMT2300A_GoRx();//CMT2300A_GoSleep();
            return 1;
        }
        if (delay == 0) {
            CMT2300A_GoStby();
            CMT2300A_EnableReadFifo();
            CMT2300A_ClearInterruptFlags();
            CMT2300A_ClearRxFifo();
            CMT2300A_GoRx();//CMT2300A_GoSleep();
            return 0;
        }
        FL_DelayMs(1);
        delay--;
    }
}
