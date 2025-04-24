#include "Define.h"
#include "SX1276.h"

void SX1276_RF_ReceiveData(void) {
    unsigned int i;
    unsigned char Sum=0;
    //if ((SX1276_RF_RXBuffer[0] == METERID[0]) && (SX1276_RF_RXBuffer[1] == METERID[1])) {
        if (SX1276_RF_RXBuffer[2] == 0xA1) {
//            RF_ReceiveFig = 0x00;
            for (i = 0; i < 8; i++) {
                Sum = Sum + SX1276_RF_RXBuffer[i];
            }
            if (Sum == SX1276_RF_RXBuffer[8]) {
                Now_NetDevParameter.ReceiveCount++;
                Now_NetDevParameter.ReceiveFlag = true;
                if ((Now_NetDevParameter.SendCount >= 100) || (Now_NetDevParameter.ReceiveCount >= 100)) {
                    Now_NetDevParameter.SendCount = 0;
                    Now_NetDevParameter.ReceiveCount = 0;
                }
                if (Now_NetDevParameter.ReceiveCount > Now_NetDevParameter.SendCount) {
                    Now_NetDevParameter.SendCount = 0;
                    Now_NetDevParameter.ReceiveCount = 0;
                }
            }
        }
    //}
    for (i = 0; i < 32; i++) {
        SX1276_RF_RXBuffer[i] = 0x00;
    }
}
