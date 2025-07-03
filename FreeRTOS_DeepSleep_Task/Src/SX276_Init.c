#include "Define.h"
#include "SX1276.h"
#include "PublicLib_No_One.h"

void SX1276LoRa_SPI_Init(void) {
    SX1276_SCLK_L;
    SX1276_NSS_H;
    SX1276_SIMO_L;
}

void SX1276LoRa_Reset(void) {
    SX1276_RST_L;
    IncludeDelayMs(200);
    SX1276_RST_H;
    IncludeDelayMs(300);
}

void SX1276LoRa_SPI_MasterIO(unsigned char out) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        if (out & 0x80) { SX1276_SIMO_H; } else { SX1276_SIMO_L; }
        SX1276_SCLK_H;
        out = (out << 1);
        SX1276_SCLK_L;
    }
}

unsigned char SX1276LoRa_SPI_ReadByte() {
    unsigned char j;
    unsigned char i;
    j = 0;
    for (i = 0; i < 8; i++) {
        SX1276_SCLK_H;
        j = (j << 1);
        if (SX1276_SOMI_VALUE) j = j | 0x01;
        SX1276_SCLK_L;
    }
    return j;
}

void cmdSwitchEn(cmdEntype_t cmd) {
    switch (cmd) {
        case enOpen: {
            SX1276_NSS_L;
        }break;
        case enClose: {
            SX1276_NSS_H;
        }break;
        default:break;
    }
}

void cmdSwitchPA(cmdpaType_t cmd) {
    switch (cmd) {
        case rxOpen: {
          //SX1276_RXD_OUT;
        }break;
        case txOpen: {
          //SX1276_TXD_OUT;
        }break;
        default:break;
    }
}

void fqcRecvData(unsigned char * lpbuf, unsigned short len) {

}

lpCtrlTypefunc_t  ctrlTypefunc = {
   SX1276LoRa_SPI_MasterIO,
   SX1276LoRa_SPI_ReadByte,
   cmdSwitchEn,
   cmdSwitchPA,
   fqcRecvData
};

void SX1276_PWR_ON(void) {
    SX1276_PWR_H;
    IncludeDelayMs(500);
}

void SX1276_PWR_OFF(void) {
    SX1276_PWR_L;
    IncludeDelayMs(200);
}

void SX1276_RF_Open(void) {
    SX1276_PWR_ON();
    SX1276LoRa_SPI_Init();
    register_rf_func(&ctrlTypefunc);
    SX1276LoRa_Reset();
    SX1276LoRaInit();
}

void SX1276_RF_Close() {
    SX1276_PWR_OFF();
    SX1276_SCLK_L;
    SX1276_NSS_L;
    SX1276_SIMO_L;
    SX1276_RST_L;
}
