#include "Define.h"
#include "SX1276.h"
#include "PublicLib_No_One.h"

extern void SX1276Init(void);  //from SX1276.c
extern void SX1276_RF_SEELP(void);  //from SX1276.c

void SX1276_RF_SpiInit(void) {
    SX1276_SCLK_L;
    SX1276_NSS_H;
    SX1276_MOSI_L;
}

void SX1276Reset(void) {
    SX1276_RST_L;
    IncludeDelayMs(200);
    SX1276_RST_H;
    IncludeDelayMs(300);
}

void SX1276_RF_SPI_MasterIO(unsigned char out) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        if (out & 0x80) { SX1276_MOSI_H; } else { SX1276_MOSI_L; }
        SX1276_SCLK_H;
        out = (out << 1);
        SX1276_SCLK_L;
    }
}

unsigned char SX1276_RF_SPI_READ_BYTE() {
    unsigned char j;
    unsigned char i;
    j = 0;
    for (i = 0; i < 8; i++) {
        SX1276_SCLK_H;
        j = (j << 1);
        if (SX1276_MISO_VALUE) j = j | 0x01;
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
   SX1276_RF_SPI_MasterIO,
   SX1276_RF_SPI_READ_BYTE,
   cmdSwitchEn,
   cmdSwitchPA,
   fqcRecvData
};

void SX1276_PWR_ON(void) {
    SX1276_PWR_H;
    IncludeDelayMs(20);
}

void SX1276_PWR_OFF(void) {
    SX1276_PWR_L;
    SX1276_NSS_L;
    IncludeDelayMs(200);
}

void SX1276_RF_OPEN(void) {
    SX1276_PWR_ON();
    IncludeDelayMs(50);
    SX1276_RF_SpiInit();
    register_rf_func(&ctrlTypefunc);
    SX1276Reset();
    SX1276Init();
}

void SX1276_RF_CLOSE() {
    SX1276_PWR_OFF();
    IncludeDelayMs(50);
    SX1276_SCLK_L;
    SX1276_NSS_L;
    SX1276_MOSI_L;
    SX1276_RST_L;
}
