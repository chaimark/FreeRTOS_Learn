#include "SX1276.h"
#include "Define.h"
#include "AT24CXXDataLoader.h"

unsigned char Sending = 0;
unsigned char Alarm_Falge = 0;
extern unsigned char RF_TXBuffer[30];
extern void SX1276_RF_SENDPACKET(unsigned char * RF_TRAN_P, unsigned char LEN);
extern void SX1276_RF_OPEN(void);
extern void SX1276_RF_CLOSE(void);
void Send_RF(void) {
    unsigned int App_Count = 400;
    unsigned char  i = 1;
    Now_NetDevParameter.SendCount++;
    if ((Now_NetDevParameter.SendCount >= 100) || (Now_NetDevParameter.ReceiveCount >= 100)) {
        Now_NetDevParameter.SendCount = 0;
        Now_NetDevParameter.ReceiveCount = 0;
    }
    if (Now_NetDevParameter.ReceiveCount > Now_NetDevParameter.SendCount) {
        Now_NetDevParameter.SendCount = 0;
        Now_NetDevParameter.ReceiveCount = 0;
    }
    SingalStatus = 0;
    Sending = 1;
    RF_ReceiveFig = 1;
    SX1276_RF_OPEN();
    SX1276_RF_SENDPACKET(RF_TXBuffer, 21);
    while (i) {
        App_Count--;
        if (RF_ReceiveFig == 0x00) {
            i = 0;
        }
        if (App_Count == 0) {
            i = 0;
        }
        IncludeDelayMs(4);
    }
    SX1276_RF_CLOSE();
    IncludeDelayMs(100);
    Alarm_Falge = 0;
    Sending = 0;
}
void Send_RF_DATA_AT_The_Beginning(void) {
    unsigned char i, Sum = 0;
    RF_Is_Beginning_Flage = 1;
    RF_TXBuffer[0] = 0x12;
    RF_TXBuffer[1] = 0x34;
    RF_TXBuffer[2] = 0x0A;
    RF_TXBuffer[3] = 0x00;
    RF_TXBuffer[4] = 0x00;
    RF_TXBuffer[5] = Now_DEV_Volt >> 8;
    RF_TXBuffer[6] = Now_DEV_Volt & 0XFF;
    RF_TXBuffer[7] = AT24CXX_Manager_NET.Meter_Type;    //仪表类型  1：PT1000,2:压力	
    RF_TXBuffer[8] = (unsigned int)Now_TemperOrPress >> 8;
    RF_TXBuffer[9] = (unsigned int)Now_TemperOrPress & 0XFF;
    RF_TXBuffer[10] = (unsigned int)Now_TemperOrPress >> 8;
    RF_TXBuffer[11] = (unsigned int)Now_TemperOrPress & 0XFF;
    RF_TXBuffer[12] = TEMP_R_VALUE_PT1 >> 8;
    RF_TXBuffer[13] = TEMP_R_VALUE_PT1 & 0XFF;
    RF_TXBuffer[14] = PressVotage >> 8;     //实际值
    RF_TXBuffer[15] = PressVotage & 0XFF;
    RF_TXBuffer[16] = AT24CXX_Manager_NET.Test_TemperOrPress_Interval >> 8;   //测量周期  分钟
    RF_TXBuffer[17] = AT24CXX_Manager_NET.Test_TemperOrPress_Interval & 0XFF;
    RF_TXBuffer[18] = AT24CXX_Manager_NET.SendInterval >> 8;  //无线发送周期  分钟
    RF_TXBuffer[19] = AT24CXX_Manager_NET.SendInterval & 0XFF;
    for (i = 0; i < 20; i++) {
        Sum = Sum + RF_TXBuffer[i];
    }
    RF_TXBuffer[20] = Sum;
    Send_RF();
    RF_Is_Beginning_Flage = 0;
}

void Send_RF_DATA(unsigned char FA_ACTIVE) {
    unsigned char i = 0, j = 0, Sum = 0;
    // RF_TXBuffer[0] = METERID[0];
    // RF_TXBuffer[1] = METERID[1];
    // RF_TXBuffer[2] = FA_ACTIVE;
    // RF_TXBuffer[3] = FM33LE02X_VREFP_Voltage >> 8;
    // RF_TXBuffer[4] = FM33LE02X_VREFP_Voltage & 0XFF;
    // RF_TXBuffer[5] = BatVotage >> 8;
    // RF_TXBuffer[6] = BatVotage & 0XFF;
    // RF_TXBuffer[7] = Meter_Type;    //仪表类型  1：PT1000,2:压力	
    // RF_TXBuffer[8] = PT1000_T2 >> 8;
    // RF_TXBuffer[9] = PT1000_T2 & 0XFF;
    // RF_TXBuffer[10] = PressValue >> 8;
    // RF_TXBuffer[11] = PressValue & 0XFF;
    // RF_TXBuffer[12] = TEMP_VALUE_PS2 >> 8;    //实际值
    // RF_TXBuffer[13] = TEMP_VALUE_PS2 & 0XFF;
    // RF_TXBuffer[14] = PressVotage >> 8;    //实际值
    // RF_TXBuffer[15] = PressVotage & 0XFF;
    // RF_TXBuffer[16] = Test_PT1000_Press_Interval >> 8;   //测量周期  秒
    // RF_TXBuffer[17] = Test_PT1000_Press_Interval & 0XFF;
    // RF_TXBuffer[18] = Send_RF_Data_Interval >> 8;  //无线发送周期  秒
    // RF_TXBuffer[19] = Send_RF_Data_Interval & 0XFF;
  //////////////////////////////
    RF_TXBuffer[j++] = METERID[0];
    RF_TXBuffer[j++] = METERID[1];
    RF_TXBuffer[j++] = FA_ACTIVE;
    RF_TXBuffer[j++] = 0x00;
    RF_TXBuffer[j++] = 0x00;
    RF_TXBuffer[j++] = Now_DEV_Volt >> 8;
    RF_TXBuffer[j++] = Now_DEV_Volt & 0XFF;
    RF_TXBuffer[j++] = AT24CXX_Manager_NET.Meter_Type;    //仪表类型  1：PT1000,2:压力
    // if (AT24CXX_Manager_NET.Meter_Type == 1) {  // 1：PT1000, 2:压力
    //     RF_TXBuffer[j++] = (unsigned int)Now_TemperOrPress >> 8;
    //     RF_TXBuffer[j++] = (unsigned int)Now_TemperOrPress & 0XFF;
    //     RF_TXBuffer[j++] = TEMP_R_VALUE_PT1 >> 8;
    //     RF_TXBuffer[j++] = TEMP_R_VALUE_PT1 & 0XFF;
    // } else {
    //     RF_TXBuffer[j++] = (unsigned int)Now_TemperOrPress >> 8;
    //     RF_TXBuffer[j++] = (unsigned int)Now_TemperOrPress & 0XFF;
    //     RF_TXBuffer[j++] = PressVotage >> 8;    //实际值
    //     RF_TXBuffer[j++] = PressVotage & 0XFF;
    // }
    RF_TXBuffer[j++] = (unsigned int)Now_TemperOrPress >> 8;
    RF_TXBuffer[j++] = (unsigned int)Now_TemperOrPress & 0XFF;
    RF_TXBuffer[j++] = TEMP_R_VALUE_PT1 >> 8;
    RF_TXBuffer[j++] = TEMP_R_VALUE_PT1 & 0XFF;
    RF_TXBuffer[j++] = (unsigned int)Now_TemperOrPress >> 8;
    RF_TXBuffer[j++] = (unsigned int)Now_TemperOrPress & 0XFF;
    RF_TXBuffer[j++] = PressVotage >> 8;    //实际值
    RF_TXBuffer[j++] = PressVotage & 0XFF;
    RF_TXBuffer[j++] = AT24CXX_Manager_NET.Test_TemperOrPress_Interval >> 8;   //测量周期  秒
    RF_TXBuffer[j++] = AT24CXX_Manager_NET.Test_TemperOrPress_Interval & 0XFF;
    RF_TXBuffer[j++] = AT24CXX_Manager_NET.SendInterval >> 8;  //无线发送周期  秒
    RF_TXBuffer[j++] = AT24CXX_Manager_NET.SendInterval & 0XFF;
    for (i = 0; i < j; i++) {
        Sum = Sum + RF_TXBuffer[i];
    }
    RF_TXBuffer[j++] = Sum;
    Send_RF();
}
