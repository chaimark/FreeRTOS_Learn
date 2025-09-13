#include "fm33_assert.h"
#include <string.h>
#include "StrLib.h"
#include "../Interflow/AT24CXXDataLoader.h"
#include "../Interflow/NumberBaseLib.h"
#include "../Interflow/JsonDataAnalyzeLib.h"
#include "../Interflow/HY_TXRX.h"
#include "../Interflow/NetProt_Module.h"

#define MaxSendCount_ManageObj 20
// 将数据按协议打包 （接口由 模组层调用 并 上浮到 RTOS_Task 层）
uint16_t _Module_TXD_6844_GetTx(void) {
    static char _Module_TXD_6844_TxBuf[512] = {0};
    static uint8_t SendManageObj_Count;
    unsigned char EEprom_METERID[4] = {0};
    memcpy(EEprom_METERID, AT24CXX_Manager_NET.MeterID, 4);

    int SendDataLen = 0;
    _Module_TXD_6844_TxBuf[SendDataLen++] = 0x68;
    _Module_TXD_6844_TxBuf[SendDataLen++] = 0x54;
    _Module_TXD_6844_TxBuf[SendDataLen++] = EEprom_METERID[0];
    _Module_TXD_6844_TxBuf[SendDataLen++] = EEprom_METERID[1];
    _Module_TXD_6844_TxBuf[SendDataLen++] = EEprom_METERID[2];
    _Module_TXD_6844_TxBuf[SendDataLen++] = EEprom_METERID[3];
    _Module_TXD_6844_TxBuf[SendDataLen++] = 0x00;
    _Module_TXD_6844_TxBuf[SendDataLen++] = 0x11;
    _Module_TXD_6844_TxBuf[SendDataLen++] = 0x11;
    SendDataLen++; // SendDataLen == 09 H 
    SendDataLen++; // SendDataLen == 10 L 
    _Module_TXD_6844_TxBuf[SendDataLen++] = ((CMD_CODE_WRITE >> 0x10) & 0x00FF); // CMDID
    _Module_TXD_6844_TxBuf[SendDataLen++] = ((CMD_CODE_WRITE >> 0x08) & 0x00FF); // DI0
    _Module_TXD_6844_TxBuf[SendDataLen++] = ((CMD_CODE_WRITE >> 0x00) & 0x00FF); // DI1

    // if (Meter_Manager.DataCount > 0) {
    //     SendDataLen = Add_HY_DataClass(NEW_NAME(_Module_TXD_6844_TxBuf), SendDataLen, RoomTemperArray_TAG, 0x01); // 添加汇云数据结构 室温数组
    // }
    SendDataLen = Add_HY_DataClass(NEW_NAME(_Module_TXD_6844_TxBuf), SendDataLen, ValveSturt, 0x01); // 添加汇云数据结构 
    // SendDataLen = Add_HY_DataClass(NEW_NAME(_Module_TXD_6844_TxBuf), SendDataLen, nowTime_TAG, 0x03); // 添加汇云数据结构 
    // SendDataLen = Add_HY_DataClass(NEW_NAME(_Module_TXD_6844_TxBuf), SendDataLen, voltage_TAG, 0x01); // 添加汇云数据结构 
    // SendDataLen = Add_HY_DataClass(NEW_NAME(_Module_TXD_6844_TxBuf), SendDataLen, CCID_TAG, 0x01); // 添加汇云数据结构 
    // SendDataLen = Add_HY_DataClass(NEW_NAME(_Module_TXD_6844_TxBuf), SendDataLen, IMEI_TAG, 0x01); // 添加汇云数据结构
    // SendDataLen = Add_HY_DataClass(NEW_NAME(_Module_TXD_6844_TxBuf), SendDataLen, CSQ_TAG, 0x01); // 添加汇云数据结构
    // SendDataLen = Add_HY_DataClass(NEW_NAME(_Module_TXD_6844_TxBuf), SendDataLen, RSRP_TAG, 0x01); // 添加汇云数据结构
    // SendDataLen = Add_HY_DataClass(NEW_NAME(_Module_TXD_6844_TxBuf), SendDataLen, RSRQ_TAG, 0x01); // 添加汇云数据结构

    if ((SendManageObj_Count == 0) || (_Module_Start_Flage == _Module_UserSend) || (_Module_Start_Flage == _Module_EEpromSend)) {
        SendManageObj_Count = MaxSendCount_ManageObj;
        SendDataLen = Add_HY_DataClass(NEW_NAME(_Module_TXD_6844_TxBuf), SendDataLen, SendClassObj_TAG, 0x01); // 添加汇云数据结构 0x01 上传参数
    } else {
        SendManageObj_Count--;
    }

    _Module_TXD_6844_TxBuf[0x09] = ((SendDataLen >> 8) & 0x00FF);
    _Module_TXD_6844_TxBuf[0x0A] = ((SendDataLen >> 0) & 0x00FF);
    _Module_TXD_6844_TxBuf[SendDataLen] = get_CheckSum((unsigned char *)_Module_TXD_6844_TxBuf, SendDataLen);
    _Module_TXD_6844_TxBuf[++SendDataLen] = 0x16;
    SendDataLen++;

    return PackHYDataToAtData(_Module_TXD_6844_TxBuf, SendDataLen, NEW_NAME(UART1Ddata.TxBuf));
}

// 根据协议提取符合协议的数据 （接口由 模组层调用 并 上浮到 RTOS_Task 层）
void _getHYCmdFormATStr(strnew Mather6844tr, uint16_t DataLen) {
    char * Start = NULL;
    if ((Start = strstr(Mather6844tr.Name._char, "AT+NMGR\r\n") + strlen("AT+NMGR\r\n")) == NULL) {
        return;
    }
    if ((Start = strstr(Start, ",6854")) == NULL) {
        return;
    }
    Start++;
    Mather6844tr.MaxLen = DataLen - (Start - Mather6844tr.Name._char);
    Mather6844tr.Name._char = Start;
    CmdTable.RxLen = ASCIIToHEX2(Mather6844tr.Name._char, Mather6844tr.MaxLen, CmdTable.RxBuf, LONG_UARTMAX);
}
// 将协议数据打包成 AT 格式 （接口由 RTOS_Task 层直接调用,再推向模组）
uint16_t PackHYDataToAtData(char * Databuf, uint16_t SendDataLen, strnew OutBuf) {
    strnew TempBuff = NEW_NAME(CmdTable.RxBuf);
    memset(TempBuff.Name._char, 0, TempBuff.MaxLen);
    HEX2ToASCII(Databuf, SendDataLen, TempBuff.Name._char, TempBuff.MaxLen);
    if (OutBuf.MaxLen < (SendDataLen * 2)) {
        return 0;
    }
    sprintf(OutBuf.Name._char, "AT+NMGS=%d,%s,100\r\n", (SendDataLen * 2), TempBuff.Name._char);
    return strlen(OutBuf.Name._char);
}

