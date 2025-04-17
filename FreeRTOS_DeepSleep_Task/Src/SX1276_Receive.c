#include "Define.h"
#include "SX1276.h"

extern unsigned char RF_TXBuffer[30];  // from RF_CC115L.c             
extern unsigned char RF_RXBuffer[50];  // from RF_CC115L.c
extern unsigned char SingalStatus;
extern unsigned char RF_ReceiveFig;

unsigned char CheckSum_RF_RXBuffer(int Len) {
    unsigned char buf;
    unsigned char i;
    buf = RF_RXBuffer[0];
    for (i = 1; i < Len; i++) {
        buf = RF_RXBuffer[i] + buf;
    }
    return buf;
}

void RECEIVE_DATA(void) {
    unsigned int i;
    unsigned char Sum;
    if ((RF_RXBuffer[0] == METERID[0]) && (RF_RXBuffer[1] == METERID[1])) {
        if ((RF_RXBuffer[2] == 0xA1) || (RF_RXBuffer[2] == 0xA2)) {
            RF_ReceiveFig = 0x00;
            Sum = 0;
            for (i = 0; i < 8; i++) {
                Sum = Sum + RF_RXBuffer[i];
            }
            if (Sum == RF_RXBuffer[8]) {
                Now_NetDevParameter.ReceiveCount++;
                if ((Now_NetDevParameter.SendCount >= 100) || (Now_NetDevParameter.ReceiveCount >= 100)) {
                    Now_NetDevParameter.SendCount = 0;
                    Now_NetDevParameter.ReceiveCount = 0;
                }
                if (Now_NetDevParameter.ReceiveCount > Now_NetDevParameter.SendCount) {
                    Now_NetDevParameter.SendCount = 0;
                    Now_NetDevParameter.ReceiveCount = 0;
                }
                SingalStatus = 0x01;
            }
        }
    }
    for (i = 0; i < 32; i++) {
        RF_RXBuffer[i] = 0x00;
    }
}
