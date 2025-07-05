#include "Define.h"
#include "RF_CMT2300A.h"
#include "CMT2300A.h"
#include "RF_CMT2300A_Receive.h"
#include "AT24CXXDataLoader.h"
#include "CMT2300A_SPI.h"
#include "TaskAboutTimer.h"

CMT2300A_T CMT2300A_T_temp;
unsigned char RF_ReceiveFig = 0x01;

bool CMT2300A_RECEIVE_DATA(void) {
    bool isSendData = false;
    if (CMT2300A_T_temp.RFRxBuf[2] != METERID[3])
        goto AA;
    if ((CMT2300A_T_temp.RFRxBuf[0] == METERID[1]) && (CMT2300A_T_temp.RFRxBuf[1] == METERID[2]) && (CMT2300A_T_temp.RFRxBuf[2] == METERID[3]) && (CMT2300A_T_temp.RFRxBuf[3] == 0x01)) {
        if (CMT2300A_T_temp.RFRxBuf[5] == 0x0A) {
            RF_ReceiveFig = 0x00;
            unsigned char Sum = 0;
            for (uint8_t i = 0; i < 16; i++) {
                Sum = Sum + CMT2300A_T_temp.RFRxBuf[i];
            }
            if (Sum == CMT2300A_T_temp.RFRxBuf[16]) {
                Get_Module_Data.CtrlDev_Now_Volt = CMT2300A_T_temp.RFRxBuf[4];
                Get_Module_Data.CtrlDev_NowRunMode = CMT2300A_T_temp.RFRxBuf[6];
                Get_Module_Data.CtrlDev_TemperRoom = CMT2300A_T_temp.RFRxBuf[8] * 256 + CMT2300A_T_temp.RFRxBuf[7];
                Get_Module_Data.CtrlDev_TemperSet = CMT2300A_T_temp.RFRxBuf[10] * 256 + CMT2300A_T_temp.RFRxBuf[9];
                Get_Module_Data.CtrlDev_Set_Degree_Part = (CMT2300A_T_temp.RFRxBuf[11] == 0x55 ? 1000 : 0);
                Get_Module_Data.CtrlDev_SendCount = CMT2300A_T_temp.RFRxBuf[12];
                Get_Module_Data.CtrlDev_ReceiveCount = CMT2300A_T_temp.RFRxBuf[13];
                Get_Module_Data.CtrlDev_CMT2300A_RF_TX_dbm = CMT2300A_T_temp.RFRxBuf[14];
                Get_Module_Data.CtrlDev_Drive_Be_Moved = CMT2300A_T_temp.RFRxBuf[15];
                isSendData = true;
                MIN_TASK.InitSetTimeTask(testConnectFor433, HourToMin_15(2), Test433AlarmTask);
            }
        }
    }
AA:
    for (uint8_t i = 0; i < 32; i++) {
        CMT2300A_T_temp.RFRxBuf[i] = 0x00;
    }
    return isSendData;
}

bool isReceiveData(void) {
    if (CMT2300A_ReadGpio2()) {
        FL_DelayMs(1);
        CMT2300A_GoStby();
        CMT2300A_ReadFifo(&CMT2300A_T_temp.RF_RxLen, 1);
        if (CMT2300A_T_temp.RF_RxLen <= 32) {
            CMT2300A_ReadFifo(CMT2300A_T_temp.RFRxBuf, CMT2300A_T_temp.RF_RxLen);
        }
        CMT2300A_ClearRxFifo();
        CMT2300A_ClearInterruptFlags();
        CMT2300A_GoRx();
        return CMT2300A_RECEIVE_DATA();
    }

    return false;
}

