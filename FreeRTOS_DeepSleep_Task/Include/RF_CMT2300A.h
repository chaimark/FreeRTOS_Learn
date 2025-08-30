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
 * @file    radio.h
 * @brief   Generic radio handlers
 *
 * @version 1.2
 * @date    Jul 17 2017
 * @author  CMOSTEK R@D
 */

#ifndef __RF_CMT2300A_H
#define __RF_CMT2300A_H

//#include "typedefs.h"
#include "CMT2300A.h"


#define C_USER_CMT2300_FIFO_LEN 32

#ifdef __cplusplus 
extern "C" {
#endif

/* RF state machine */
    typedef enum {
        RF_STATE_IDLE = 0,
        RF_STATE_RX_START,
        RF_STATE_RX_WAIT,
        RF_STATE_RX_DONE,
        RF_STATE_RX_TIMEOUT,
        RF_STATE_TX_START,
        RF_STATE_TX_WAIT,
        RF_STATE_TX_DONE,
        RF_STATE_TX_TIMEOUT,
        RF_STATE_ERROR,
    } EnumRFStatus;

    /* RF process function results */
    typedef enum {
        RF_IDLE = 0,
        RF_BUSY,
        RF_RX_DONE,
        RF_RX_TIMEOUT,
        RF_TX_DONE,
        RF_TX_TIMEOUT,
        RF_ERROR,
    } EnumRFResult;

    typedef enum RF_statiut {
        Rf_Rx = 0,
        Rf_Tx = 1,
        Rf_Stby = 2,
        Rf_Sleep = 3,
    } RF_statiut_t;

    typedef struct {
        RF_statiut_t Rf_statiu;
        unsigned char Work_Mode;
        unsigned char syncvalue[8];
        unsigned char RF_Recv_end;
        unsigned char RF_Send_end;
        unsigned char RF_RxLen;
        unsigned char RF_TxLen;
        unsigned char RFTxBuf[C_USER_CMT2300_FIFO_LEN];
        unsigned char RFRxBuf[C_USER_CMT2300_FIFO_LEN];
    }CMT2300A_T;

    void RF_Init(void);
    void RF_Config(void);
    
#ifdef __cplusplus 
}
#endif
// RF_CMT2300A
extern CMT2300A_T CMT2300A_T_temp;
extern unsigned char Radio_Send_FixedLen(const unsigned char pBuf[], unsigned char len);

#endif

