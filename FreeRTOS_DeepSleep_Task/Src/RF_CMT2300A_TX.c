#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Define.h"
#include "CMT2300A.h"
#include "CMT2300A_SPI.h"
#include "RF_CMT2300A.h"
#include "RF_CMT2300A_Init.h"
#include "RF_CMT2300A_TX.h"
#include "RF_CMT2300A_Receive.h"
#include "Display.h"
#include "../Interflow/PublicLib_No_One.h"


void Send_RF(unsigned char FA_ACTIVE) {
    unsigned int delay = 0;
    ShowSignal(true);
    RF_ReceiveFig = 1;
    CMT2300A_RF_OPEN();
    Get_Module_Data.CtrlDev_CMT2300A_RF_TX_dbm = 0;
    Radio_Send_FixedLen(CMT2300A_T_temp.RFTxBuf, 9);
    delay = 900;
    while (1) {
        if (CMT2300A_ReadGpio2()) {
            FL_DelayMs(1);
            CMT2300A_GoStby();
            Get_Module_Data.CtrlDev_CMT2300A_RF_TX_dbm = CMT2300A_GetRssiDBm();
            CMT2300A_ReadFifo(&CMT2300A_T_temp.RF_RxLen, 1);
            if (CMT2300A_T_temp.RF_RxLen > 32)
                CMT2300A_ReadFifo(CMT2300A_T_temp.RFRxBuf, 32);
            CMT2300A_ReadFifo(CMT2300A_T_temp.RFRxBuf, CMT2300A_T_temp.RF_RxLen);
            CMT2300A_ClearRxFifo();
            CMT2300A_ClearInterruptFlags();
            CMT2300A_GoRx();
            CMT2300A_RECEIVE_DATA();
        }
        if (RF_ReceiveFig == 0x00) break;
        if (delay == 0) break;
        FL_DelayMs(1);
        delay--;
    }
    // CMT2300A_RF_CLOSE();
    ShowSignal(false);
}

//**********************************
void Send_RF_DATA_AT_The_Beginning(void) {
    unsigned char Sum = 0;
    unsigned char i;
    unsigned char CAIJIQITEMP;
    CAIJIQITEMP = METERID[3];
    METERID[3] = 0x91;
    CMT2300A_T_temp.RFTxBuf[0] = METERID[1];
    CMT2300A_T_temp.RFTxBuf[1] = METERID[2];
    CMT2300A_T_temp.RFTxBuf[2] = METERID[3];
    CMT2300A_T_temp.RFTxBuf[3] = 0x01;
    CMT2300A_T_temp.RFTxBuf[4] = 0xA1;
    CMT2300A_T_temp.RFTxBuf[5] = (Get_Module_Data.Now_Degree_Part < 100 ? 0x99 : 0x55);
    CMT2300A_T_temp.RFTxBuf[6] = 0x01;
    CMT2300A_T_temp.RFTxBuf[7] = 0x05;
    for (i = 0; i < 8; i++) {
        Sum = CMT2300A_T_temp.RFTxBuf[i] + Sum;
    }
    CMT2300A_T_temp.RFTxBuf[8] = Sum;
    Send_RF(0x0A);
    METERID[3] = CAIJIQITEMP;
}

void Send_RF_DATA(unsigned char FA_ACTIVE) {
    unsigned char Sum = 0;
    unsigned char i;
    CMT2300A_T_temp.RFTxBuf[0] = METERID[1];
    CMT2300A_T_temp.RFTxBuf[1] = METERID[2];
    CMT2300A_T_temp.RFTxBuf[2] = METERID[3];
    CMT2300A_T_temp.RFTxBuf[3] = 0x01;
    CMT2300A_T_temp.RFTxBuf[4] = 0xA1;
    CMT2300A_T_temp.RFTxBuf[5] = (Get_Module_Data.Now_Degree_Part < 100 ? 0x99 : 0x55);
    CMT2300A_T_temp.RFTxBuf[6] = 0x01;
    CMT2300A_T_temp.RFTxBuf[7] = 0x05;
    for (i = 0; i < 8; i++) {
        Sum = CMT2300A_T_temp.RFTxBuf[i] + Sum;
    }
    CMT2300A_T_temp.RFTxBuf[8] = Sum;
    Send_RF(FA_ACTIVE);
}

bool Module_Send_Data(void) {
    Send_RF_DATA(0x0A);
    return true;
}


