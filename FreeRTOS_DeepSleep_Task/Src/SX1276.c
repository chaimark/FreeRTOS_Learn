#include "SX1276.h"
#include "Define.h"

unsigned int TEMP_R_VALUE_PT1;
unsigned int TEMP_R_VALUE_PT2;
unsigned char Current_meter_status;
unsigned int PressVotage;   // 压力表的电压 Now_DEV_Volt
unsigned char RF_ReceiveFig = 0x01;
unsigned char SingalStatus = 0;

const unsigned char SpreadingFactor = 11;    //7-12
const unsigned char CodingRate = 2;          //1-4
const unsigned char Bw_Frequency = 7;        //6-9
const unsigned char powerValue = 7;
const unsigned char power_data[8] = {0X80,0X80,0X80,0X83,0X86,0x89,0x8c,0x8f};
unsigned char SX1276_RF_RXBuffer[32];
unsigned char Frequency[3] = {0x6C,0x80,0x00}; //433
unsigned char RF_RXBuffer[50];
unsigned char RF_TXBuffer[30];
unsigned char RF_EX0_STATUS;
unsigned char CRC_Value;
unsigned char SX1278_RLEN;

extern unsigned char Alarm_Falge;
extern void RECEIVE_DATA(void);
lpCtrlTypefunc_t lpTypefunc = {0,0,0,0,0};
void register_rf_func(lpCtrlTypefunc_t * func) {
    if (func->lpByteWritefunc != 0) {
        lpTypefunc.lpByteWritefunc = func->lpByteWritefunc;
    }
    if (func->lpByteReadfunc != 0) {
        lpTypefunc.lpByteReadfunc = func->lpByteReadfunc;
    }
    if (func->lpSwitchEnStatus != 0) {
        lpTypefunc.lpSwitchEnStatus = func->lpSwitchEnStatus;
    }
    if (func->paSwitchCmdfunc != 0) {
        lpTypefunc.paSwitchCmdfunc = func->paSwitchCmdfunc;
    }
    if (func->lpRecvDataTousr != 0) {
        lpTypefunc.lpRecvDataTousr = func->lpRecvDataTousr;
    }
}

void FSK_SEND_PACKET(void) {
    SX1276LoRaSetOpMode(Stdby_mode);
    SX1276WriteBuffer(REG_LR_DIOMAPPING1, 0x01);
    SX1276WriteBuffer(REG_LR_DIOMAPPING2, 0x20);
}

void SX1276WriteBuffer(unsigned char addr, unsigned char buffer) {
    lpTypefunc.lpSwitchEnStatus(enOpen); //NSS = 0;
    lpTypefunc.lpByteWritefunc(addr | 0x80);
    lpTypefunc.lpByteWritefunc(buffer);
    lpTypefunc.lpSwitchEnStatus(enClose); //NSS = 1;
}

unsigned char SX1276ReadBuffer(unsigned char addr) {
    unsigned char Value;
    lpTypefunc.lpSwitchEnStatus(enOpen); //NSS = 0;
    lpTypefunc.lpByteWritefunc(addr & 0x7f);
    Value = lpTypefunc.lpByteReadfunc();
    lpTypefunc.lpSwitchEnStatus(enClose);//NSS = 1;
    return Value;
}

void SX1276LoRaSetOpMode(RFMode_SET opMode) {
    unsigned char opModePrev;
    opModePrev = SX1276ReadBuffer(REG_LR_OPMODE);
    opModePrev &= 0xf8;
    opModePrev |= (unsigned char)opMode;
    SX1276WriteBuffer(REG_LR_OPMODE, opModePrev);
}

void SX1276LoRaFsk(Debugging_fsk_ook opMode) {
    unsigned char opModePrev;
    opModePrev = SX1276ReadBuffer(REG_LR_OPMODE);
    opModePrev &= 0x7F;
    opModePrev |= (unsigned char)opMode;
    SX1276WriteBuffer(REG_LR_OPMODE, opModePrev);
}

void SX1276LoRaSetRFFrequency(void) {
    unsigned int RFFrequencyTemp = 0;
    RFFrequencyTemp = AT24CXX_Manager_NET.FrequencyPoint;
    // unsigned char RFFrequency = 0x00;
    // if (RF_Is_Beginning_Flage == 1) {
    //     if ((RFFrequency > 0x00) && (RFFrequency <= 0x25)) {
    //         RFFrequencyTemp = ((METERID[0] & 0xF0) >> 4) * 10 + (METERID[0] & 0x0F);
    //     }
    //     if ((RFFrequency > 0x25) && (RFFrequency <= 0x50)) {
    //         RFFrequencyTemp = ((METERID[0] & 0xF0) >> 4) * 10 + (METERID[0] & 0x0F) - 25;
    //     }
    //     if ((RFFrequency > 0x50) && (RFFrequency <= 0x75)) {
    //         RFFrequencyTemp = ((METERID[0] & 0xF0) >> 4) * 10 + (METERID[0] & 0x0F) - 50;
    //     }
    //     if ((RFFrequency > 0x75) && (RFFrequency <= 0x99)) {
    //         RFFrequencyTemp = ((METERID[0] & 0xF0) >> 4) * 10 + (METERID[0] & 0x0F) - 75;
    //     }
    // } else {
    //     RFFrequencyTemp = 0;
    // }
    if (Alarm_Falge != 0)
        RFFrequencyTemp = RFFrequencyTemp + 3;
    else
        RFFrequencyTemp = RFFrequencyTemp;
    switch (RFFrequencyTemp) {  //Frequency[3]={0x6c,0x80,0x00};
        case 0:
            Frequency[0] = 0x6c;  //433.99  7110656+16384
            Frequency[1] = 0x80;
            Frequency[2] = 0x00;
            break;
        case 1:
            Frequency[0] = 0x6c;  //434.99  
            Frequency[1] = 0xC0;
            Frequency[2] = 0x00;
            break;
        case 2:
            Frequency[0] = 0x6D;  //435.99
            Frequency[1] = 0x00;
            Frequency[2] = 0x00;
            break;
        case 3:
            Frequency[0] = 0x6D;  //436.99
            Frequency[1] = 0x40;
            Frequency[2] = 0x00;
            break;
        case 4:
            Frequency[0] = 0x6D;  //437.99
            Frequency[1] = 0x80;
            Frequency[2] = 0x00;
            break;
        case 5:
            Frequency[0] = 0x6D;  //438.99
            Frequency[1] = 0xC0;
            Frequency[2] = 0x00;
            break;
        case 6:
            Frequency[0] = 0x6E;  //439.99
            Frequency[1] = 0x00;
            Frequency[2] = 0x00;
            break;
        case 7:
            Frequency[0] = 0x6E;  //440.99
            Frequency[1] = 0x40;
            Frequency[2] = 0x00;
            break;
        case 8:
            Frequency[0] = 0x6E;  //441.99
            Frequency[1] = 0x80;
            Frequency[2] = 0x00;
            break;
        case 9:
            Frequency[0] = 0x6E;  //442.99
            Frequency[1] = 0xC0;
            Frequency[2] = 0x00;
            break;
        case 10:
            Frequency[0] = 0x6F;  //443.99
            Frequency[1] = 0x00;
            Frequency[2] = 0x00;
            break;
        case 11:
            Frequency[0] = 0x6F;  //444.99  
            Frequency[1] = 0x40;
            Frequency[2] = 0x00;
            break;
        case 12:
            Frequency[0] = 0x6F;  //445.99  
            Frequency[1] = 0x80;
            Frequency[2] = 0x00;
            break;
        case 13:
            Frequency[0] = 0x6F;  //446.99
            Frequency[1] = 0xC0;
            Frequency[2] = 0x00;
            break;
        case 14:
            Frequency[0] = 0x70;  //447.99
            Frequency[1] = 0x00;
            Frequency[2] = 0x00;
            break;
        case 15:
            Frequency[0] = 0x70;  //448.99
            Frequency[1] = 0x40;
            Frequency[2] = 0x00;
            break;
        case 16:
            Frequency[0] = 0x70;  //449.99
            Frequency[1] = 0x80;
            Frequency[2] = 0x00;
            break;
        case 17:
            Frequency[0] = 0x70;  //450.99
            Frequency[1] = 0xC0;
            Frequency[2] = 0x00;
            break;
        case 18:
            Frequency[0] = 0x71;  //451.99
            Frequency[1] = 0x00;
            Frequency[2] = 0x00;
            break;
        case 19:
            Frequency[0] = 0x71;  //452.99
            Frequency[1] = 0x40;
            Frequency[2] = 0x00;
            break;
        case 20:
            Frequency[0] = 0x71;  //453.99
            Frequency[1] = 0x80;
            Frequency[2] = 0x00;
            break;
        case 21:
            Frequency[0] = 0x71;  //455.99  
            Frequency[1] = 0xC0;
            Frequency[2] = 0x00;
            break;
        case 22:
            Frequency[0] = 0x72;  //456.99  
            Frequency[1] = 0x00;
            Frequency[2] = 0x00;
            break;
        case 23:
            Frequency[0] = 0x72;  //457.99
            Frequency[1] = 0x40;
            Frequency[2] = 0x00;
            break;
        case 24:
            Frequency[0] = 0x72;  //458.99
            Frequency[1] = 0x80;
            Frequency[2] = 0x00;
            break;
        case 25:
            Frequency[0] = 0x72;  //459.99  
            Frequency[1] = 0xC0;
            Frequency[2] = 0x00;
            break;
        default:
            Frequency[0] = 0x6c;  //433.99  7110656+16384
            Frequency[1] = 0x80;
            Frequency[2] = 0x00;
            break;
    }
    SX1276WriteBuffer(REG_LR_FRFMSB, Frequency[0]);
    SX1276WriteBuffer(REG_LR_FRFMID, Frequency[1]);
    SX1276WriteBuffer(REG_LR_FRFLSB, Frequency[2]);
}

void SX1276LoRaSetRFPower(unsigned char power) {
    SX1276WriteBuffer(REG_LR_PADAC, 0x87);
    SX1276WriteBuffer(REG_LR_PACONFIG, power_data[power]);
}

void SX1276LoRaSetSpreadingFactor(unsigned char factor) {
    unsigned char RECVER_DAT;
    SX1276LoRaSetNbTrigPeaks(3);
    RECVER_DAT = SX1276ReadBuffer(REG_LR_MODEMCONFIG2);
    RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG2_SF_MASK) | (factor << 4);
    SX1276WriteBuffer(REG_LR_MODEMCONFIG2, RECVER_DAT);
}

void SX1276LoRaSetNbTrigPeaks(unsigned char value) {
    unsigned char RECVER_DAT;
    RECVER_DAT = SX1276ReadBuffer(0x31);
    RECVER_DAT = (RECVER_DAT & 0xF8) | value;
    SX1276WriteBuffer(0x31, RECVER_DAT);
}

void SX1276LoRaSetErrorCoding(unsigned char value) {
    unsigned char RECVER_DAT;
    RECVER_DAT = SX1276ReadBuffer(REG_LR_MODEMCONFIG1);
    RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG1_CODINGRATE_MASK) | (value << 1);
    SX1276WriteBuffer(REG_LR_MODEMCONFIG1, RECVER_DAT);
    // LoRaSettings.ErrorCoding = value;
}

void SX1276LoRaSetPacketCrcOn(BOOL enable) {
    unsigned char RECVER_DAT;
    RECVER_DAT = SX1276ReadBuffer(REG_LR_MODEMCONFIG2);
    RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK) | (enable << 2);
    SX1276WriteBuffer(REG_LR_MODEMCONFIG2, RECVER_DAT);
}

void SX1276LoRaSetSignalBandwidth(unsigned char bw) {
    unsigned char RECVER_DAT;
    RECVER_DAT = SX1276ReadBuffer(REG_LR_MODEMCONFIG1);
    RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG1_BW_MASK) | (bw << 4);
    SX1276WriteBuffer(REG_LR_MODEMCONFIG1, RECVER_DAT);
    // LoRaSettings.SignalBw = bw;
}

void SX1276LoRaSetImplicitHeaderOn(BOOL enable) {
    unsigned char RECVER_DAT;
    RECVER_DAT = SX1276ReadBuffer(REG_LR_MODEMCONFIG1);
    RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK) | (enable);
    SX1276WriteBuffer(REG_LR_MODEMCONFIG1, RECVER_DAT);
}

void SX1276LoRaSetSymbTimeout(unsigned int value) {
    unsigned char RECVER_DAT[2];
    RECVER_DAT[0] = SX1276ReadBuffer(REG_LR_MODEMCONFIG2);
    RECVER_DAT[1] = SX1276ReadBuffer(REG_LR_SYMBTIMEOUTLSB);
    RECVER_DAT[0] = (RECVER_DAT[0] & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK) | ((value >> 8) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK);
    RECVER_DAT[1] = value & 0xFF;
    SX1276WriteBuffer(REG_LR_MODEMCONFIG2, RECVER_DAT[0]);
    SX1276WriteBuffer(REG_LR_SYMBTIMEOUTLSB, RECVER_DAT[1]);
}

void SX1276LoRaSetPayloadLength(unsigned char value) {
    SX1276WriteBuffer(REG_LR_PAYLOADLENGTH, value);
}

void SX1276LoRaSetPreamLength(unsigned int value) {
    unsigned char a[2];
    a[0] = (value & 0xff00) >> 8;
    a[1] = value & 0xff;
    SX1276WriteBuffer(REG_LR_PREAMBLEMSB, a[0]);
    SX1276WriteBuffer(REG_LR_PREAMBLELSB, a[1]);
}

void SX1276LoRaSetMobileNode(BOOL enable) {
    unsigned char RECVER_DAT;
    RECVER_DAT = SX1276ReadBuffer(REG_LR_MODEMCONFIG3);
    RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG3_MOBILE_NODE_MASK) | (enable << 3);
    SX1276WriteBuffer(REG_LR_MODEMCONFIG3, RECVER_DAT);
}

void SX1276Init(void) {
    SX1276LoRaSetOpMode(Sleep_mode);
    SX1276LoRaFsk(LORA_mode);
    SX1276LoRaSetOpMode(Stdby_mode);
    SX1276WriteBuffer(REG_LR_DIOMAPPING1, GPIO_VARE_1);
    SX1276WriteBuffer(REG_LR_DIOMAPPING2, GPIO_VARE_2);
    SX1276LoRaSetRFFrequency();
    SX1276LoRaSetRFPower(powerValue);
    SX1276LoRaSetSpreadingFactor(SpreadingFactor);
    SX1276LoRaSetErrorCoding(CodingRate);
    SX1276LoRaSetPacketCrcOn(TRUE);
    SX1276LoRaSetSignalBandwidth(Bw_Frequency);
    SX1276LoRaSetImplicitHeaderOn(FLASE);
    SX1276LoRaSetPayloadLength(0xff);
    SX1276LoRaSetSymbTimeout(0x3FF);
    SX1276LoRaSetMobileNode(TRUE);
    SX1276_RF_RECEIVE();
    //RF_SEELP();
}

void SX1276_RF_SENDPACKET(unsigned char * RF_TRAN_P, unsigned char LEN) {
    unsigned char ASM_i;
    lpTypefunc.paSwitchCmdfunc(txOpen);
    SX1276LoRaSetOpMode(Stdby_mode);
    SX1276WriteBuffer(REG_LR_HOPPERIOD, 0);
    SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_TXD_Value);
    SX1276WriteBuffer(REG_LR_PAYLOADLENGTH, LEN);
    SX1276WriteBuffer(REG_LR_FIFOTXBASEADDR, 0);
    SX1276WriteBuffer(REG_LR_FIFOADDRPTR, 0);
    lpTypefunc.lpSwitchEnStatus(enOpen);
    lpTypefunc.lpByteWritefunc(0x80);
    for (ASM_i = 0; ASM_i < LEN; ASM_i++) {
        lpTypefunc.lpByteWritefunc(*RF_TRAN_P);
        RF_TRAN_P++;
    }
    lpTypefunc.lpSwitchEnStatus(enClose);
    SX1276WriteBuffer(REG_LR_DIOMAPPING1, 0x40);
    SX1276WriteBuffer(REG_LR_DIOMAPPING2, 0x00);
    SX1276LoRaSetOpMode(Transmitter_mode);
}

void SX1276_RF_RECEIVE(void) {
    SX1276LoRaSetOpMode(Stdby_mode);
    SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_RXD_Value);
    SX1276WriteBuffer(REG_LR_HOPPERIOD, PACKET_MIAX_Value);
    SX1276WriteBuffer(REG_LR_DIOMAPPING1, 0X00);
    SX1276WriteBuffer(REG_LR_DIOMAPPING2, 0X00);
    SX1276LoRaSetOpMode(Receiver_mode);
    lpTypefunc.paSwitchCmdfunc(rxOpen);
}

void SX1276_RF_CAD_RECEIVE(void) {
    SX1276LoRaSetOpMode(Stdby_mode);
    SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_CAD_Value);
    SX1276WriteBuffer(REG_LR_DIOMAPPING1, 0X80);
    SX1276WriteBuffer(REG_LR_DIOMAPPING2, 0X00);
    SX1276LoRaSetOpMode(CAD_mode);
    lpTypefunc.paSwitchCmdfunc(rxOpen);
}

void SX1276_RF_SEELP(void) {
    SX1276LoRaSetOpMode(Stdby_mode);
    SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_SEELP_Value);
    SX1276WriteBuffer(REG_LR_DIOMAPPING1, 0X00);
    SX1276WriteBuffer(REG_LR_DIOMAPPING2, 0X00);
    SX1276LoRaSetOpMode(Sleep_mode);
}

void SX1276_Interupt(void) {
    unsigned int	i;
    unsigned char RF_REC_RLEN_i;
    RF_EX0_STATUS = SX1276ReadBuffer(REG_LR_IRQFLAGS);
    if ((RF_EX0_STATUS & 0x08) == 0x08) {
        SX1276_RF_RECEIVE();
    }
    if ((RF_EX0_STATUS & 0x40) == 0x40) {
        CRC_Value = SX1276ReadBuffer(REG_LR_MODEMCONFIG2);
        if ((CRC_Value & 0x04) == 0x04) {
            SX1276WriteBuffer(REG_LR_FIFOADDRPTR, 0x00);
            SX1278_RLEN = SX1276ReadBuffer(REG_LR_NBRXBYTES);
            lpTypefunc.lpSwitchEnStatus(enOpen);
            lpTypefunc.lpByteWritefunc(0x00);
            for (RF_REC_RLEN_i = 0; RF_REC_RLEN_i < SX1278_RLEN; RF_REC_RLEN_i++) {
                if (RF_REC_RLEN_i < 32) SX1276_RF_RXBuffer[RF_REC_RLEN_i] = lpTypefunc.lpByteReadfunc();
            }
            lpTypefunc.lpSwitchEnStatus(enClose);
        }
        for (i = 0; i < SX1278_RLEN; i++) {
            if (i < 32) RF_RXBuffer[i] = SX1276_RF_RXBuffer[i];
        }
        SX1276LoRaSetOpMode(Stdby_mode);
        RECEIVE_DATA();
    }
    SX1276WriteBuffer(REG_LR_IRQFLAGS, 0xff);
}

void SX1276_GPIO_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    // PA0_MISO
    GPIO_InitStruct.pin = FL_GPIO_PIN_0;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_0);
    // PA1_MOSI  PA2_NSS   PA3_SCLK   PA4_RF_Control    PA7_RST_433          
    GPIO_InitStruct.pin = FL_GPIO_PIN_1 | FL_GPIO_PIN_2 | FL_GPIO_PIN_3 | FL_GPIO_PIN_4 | FL_GPIO_PIN_7;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_1 | FL_GPIO_PIN_2 | FL_GPIO_PIN_3 | FL_GPIO_PIN_4 | FL_GPIO_PIN_7);
    // PB2_GD0_433    
    GPIO_InitStruct.pin = FL_GPIO_PIN_2;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

