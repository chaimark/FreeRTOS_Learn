#include "SX1276.h"
#include "PT1000.h"
#include "Define.h"
#include "AT24CXXDataLoader.h"

extern unsigned char SX1276_RF_TXBuffer[30];
extern void SX1276_RF_SendPacket(unsigned char * RF_TRAN_P, unsigned char LEN);
extern void SX1276_RF_Open(void);

void Send_RF(int DataLen) {
    Now_NetDevParameter.SendCount++;
    if ((Now_NetDevParameter.SendCount >= 100) || (Now_NetDevParameter.ReceiveCount >= 100)) {
        Now_NetDevParameter.SendCount = 0;
        Now_NetDevParameter.ReceiveCount = 0;
    }
    if (Now_NetDevParameter.ReceiveCount > Now_NetDevParameter.SendCount) {
        Now_NetDevParameter.SendCount = 0;
        Now_NetDevParameter.ReceiveCount = 0;
    }
    SX1276_RF_Open();
    Now_NetDevParameter.ReceiveFlag = false;
    SX1276_RF_SendPacket(SX1276_RF_TXBuffer, DataLen);
    for (int i = 500; i > 0; i--) {  
        if (Now_NetDevParameter.ReceiveFlag) {
            break;
        }
        IncludeDelayMs(2);
    }
    SX1276_RF_Close();
    Alarm_Falge = false;
}

void Send_RF_DATA_AT_The_Beginning(void) {
    unsigned char j = 0, Sum = 0;
    SX1276_RF_TXBuffer[j++] = METERID[0];
    SX1276_RF_TXBuffer[j++] = METERID[1];
    SX1276_RF_TXBuffer[j++] = 0x0A;
    SX1276_RF_TXBuffer[j++] = AT24CXX_Manager_NET.Meter_Type;    //仪表类型  1：PT1000,2:压力	
    SX1276_RF_TXBuffer[j++] = 0x00;
    SX1276_RF_TXBuffer[j++] = 0x00;
    SX1276_RF_TXBuffer[j++] = AT24CXX_Manager_NET.SendInterval >> 8;  //无线发送周期  秒
    SX1276_RF_TXBuffer[j++] = AT24CXX_Manager_NET.SendInterval & 0XFF;
    SX1276_RF_TXBuffer[j++] = AT24CXX_Manager_NET.Test_TemperOrPress_Interval >> 8;   //测量周期  秒
    SX1276_RF_TXBuffer[j++] = AT24CXX_Manager_NET.Test_TemperOrPress_Interval & 0XFF;
    SX1276_RF_TXBuffer[j++] = 3600 >> 8;
    SX1276_RF_TXBuffer[j++] = 3600 & 0XFF;
    SX1276_RF_TXBuffer[j++] = Current_meter_status;
    for (unsigned char i = 0; i < j; i++) {
        Sum = Sum + SX1276_RF_TXBuffer[i];
    }
    SX1276_RF_TXBuffer[j++] = Sum;
    Send_RF(j);
}

void Send_RF_DATA(unsigned char FA_ACTIVE) {
    unsigned char j = 0, Sum = 0;
    SX1276_RF_TXBuffer[j++] = METERID[0];
    SX1276_RF_TXBuffer[j++] = METERID[1];
    SX1276_RF_TXBuffer[j++] = FA_ACTIVE;
    SX1276_RF_TXBuffer[j++] = AT24CXX_Manager_NET.Meter_Type;    //仪表类型  1：PT1000,2:压力	
    SX1276_RF_TXBuffer[j++] = (unsigned int)Now_TemperOrPress >> 8;
    SX1276_RF_TXBuffer[j++] = (unsigned int)Now_TemperOrPress & 0XFF;
    SX1276_RF_TXBuffer[j++] = AT24CXX_Manager_NET.SendInterval >> 8;  //无线发送周期  秒
    SX1276_RF_TXBuffer[j++] = AT24CXX_Manager_NET.SendInterval & 0XFF;
    SX1276_RF_TXBuffer[j++] = AT24CXX_Manager_NET.Test_TemperOrPress_Interval >> 8;   //测量周期  秒
    SX1276_RF_TXBuffer[j++] = AT24CXX_Manager_NET.Test_TemperOrPress_Interval & 0XFF;
    SX1276_RF_TXBuffer[j++] = Now_DEV_Volt >> 8;
    SX1276_RF_TXBuffer[j++] = Now_DEV_Volt & 0XFF;
    SX1276_RF_TXBuffer[j++] = Current_meter_status;
    for (unsigned char i = 0; i < j; i++) {
        Sum = Sum + SX1276_RF_TXBuffer[i];
    }
    SX1276_RF_TXBuffer[j++] = Sum;
    Send_RF(j);;
}
