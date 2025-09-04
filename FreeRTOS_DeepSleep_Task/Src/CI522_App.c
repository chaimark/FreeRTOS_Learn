#include "CI522_App.h"
#include <stdlib.h>
#include <string.h>
#include "CI522_interface.h"
#include "stdio.h"
#include "SetTime.h"
#include "Define.h"
#include "NumberBaseLib.h"

void I_CI522_ClearBitMask(unsigned char reg, unsigned char mask) {
    char tmp = 0x00;
    tmp = I_CI522_IO_Read(reg);
    I_CI522_IO_Write(reg, tmp & ~mask);  // clear bit mask
}

void I_CI522_SetBitMask(unsigned char reg, unsigned char mask) {
    char tmp = 0x00;
    tmp = I_CI522_IO_Read(reg);
    I_CI522_IO_Write(reg, tmp | mask);  // set bit mask
}
/***********************************************
 * 函数名：PCDAntennaOn
 * 描述  ：开启天线  每次启动或关闭天险发射之间应至少有1ms的间隔
 * 输入  ：无
 * 返回  : 无
 * 调用  ：外部调用
 **********************************************/
void PCDAntennaOn(void) {
    unsigned char i;
    i = I_CI522_IO_Read(TxControlReg);
    if (!(i & 0x03)) {
        I_CI522_SetBitMask(TxControlReg, 0x03); // bit1 bit0 置1
    }
}

/***********************************************
 * 函数名：PCDAntennaOff
 * 描述  ：关闭天线
 * 输入  ：无
 * 返回  : 无
 * 调用  ：外部调用
 **********************************************/
void PCDAntennaOff(void) {
    I_CI522_ClearBitMask(TxControlReg, 0x03);  // bit1 bit0 清0
}
/*===============================
 函数功能：读A卡初始化配置

 ================================*/
void PCD_CI522_TypeA_Init(void) {
    I_CI522_ClearBitMask(Status2Reg, 0x08);
    // Reset baud rates
    I_CI522_IO_Write(TxModeReg, 0x00);
    I_CI522_IO_Write(RxModeReg, 0x00);
    // Reset ModWidthReg
    I_CI522_IO_Write(ModWidthReg, 0x26);
    // RxGain:110,43dB by default;
    I_CI522_IO_Write(RFCfgReg, RFCfgReg_Val);
    // When communicating with a PICC we need a timeout if something goes wrong.
    // f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
    // TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
    I_CI522_IO_Write(TModeReg, 0x80);// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
    I_CI522_IO_Write(TPrescalerReg, 0xa9);// TPreScaler = TModeReg[3..0]:TPrescalerReg
    I_CI522_IO_Write(TReloadRegH, 0x03); // Reload timer 
    I_CI522_IO_Write(TReloadRegL, 0xe8); // Reload timer 
    I_CI522_IO_Write(TxASKReg, 0x40);	// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
    I_CI522_IO_Write(ModeReg, 0x3D);	// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
    I_CI522_IO_Write(CommandReg, 0x00);  // Turn on the analog part of receiver   

    PCDAntennaOn();
}


//***********************************//修改新增内容
extern uint8_t Si522_IRQ_flagA;
UserCard CardData = {0};
/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////

/***********************************************
 * 函数名：CalulateCRC
 * 描述  ：用MF522计算CRC16函数
 * 输入  ：
 * 返回  : 无
 * 调用  ：外部调用
 **********************************************/
void CalulateCRC(unsigned char * pIndata, unsigned char len, unsigned char * pOutData) {
    unsigned char i, n;
    I_CI522_ClearBitMask(DivIrqReg, 0x04);
    I_CI522_IO_Write(CommandReg, PCD_IDLE);
    I_CI522_SetBitMask(FIFOLevelReg, 0x80);
    for (i = 0; i < len; i++) {
        I_CI522_IO_Write(FIFODataReg, *(pIndata + i));
    }
    I_CI522_IO_Write(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do {
        n = I_CI522_IO_Read(DivIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x04));
    pOutData[0] = I_CI522_IO_Read(CRCResultRegL);
    pOutData[1] = I_CI522_IO_Read(CRCResultRegH);
}

/**************************************************************************************************
 * 函数名：PCDComMF522
 * 描述  ：通过RC522和ISO14443卡通讯
 * 输入  ：Command[IN]:RC522命令字              pInData[IN]:通过RC522发送到卡片的数据    InLenByte[IN]:发送数据的字节长度
 *       ：pOutData[OUT]:接收到的卡片返回数据   pOutLenBit[OUT]:返回数据的位长度
 * 返回  : 无
 * 调用  ：外部调用
 *************************************************************************************************/
void EndRead(void) {
    I_CI522_SetBitMask(ControlReg, 0x80);           // stop timer now
    I_CI522_IO_Write(CommandReg, PCD_IDLE);
}
char PCDComMF522(unsigned char Command,
    unsigned char * pInData, unsigned char InLenByte,
    unsigned char * pOutData, unsigned int * pOutLenBit) {
    if (InLenByte == 0) {
        return MI_ERR;
    }
    char status = MI_ERR;
    unsigned char irqEn = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    switch (Command) {
        case PCD_AUTHENT:
            irqEn = 0x12;
            waitFor = 0x10;
            break;
        case PCD_TRANSCEIVE:
            irqEn = 0x77;
            waitFor = 0x30;
            break;
        default:
            break;
    }
    //I_CI522_IO_Write(ComIEnReg,irqEn|0x80);
    I_CI522_ClearBitMask(ComIrqReg, 0x80);
    I_CI522_IO_Write(CommandReg, PCD_IDLE);
    I_CI522_SetBitMask(FIFOLevelReg, 0x80);
    for (uint8_t i = 0; i < InLenByte; i++) {
        I_CI522_IO_Write(FIFODataReg, pInData[i]);
    }
    I_CI522_IO_Write(CommandReg, Command);

    if (Command == PCD_TRANSCEIVE) {
        I_CI522_SetBitMask(BitFramingReg, 0x80);
    }
    //根据时钟频率调整
    unsigned char TempChr = 0;
    SetTime.InitSetTimeTask(TempReadCrad, SecTo10Ms(0.2), NULL);
    while (SetTime.Task[TempReadCrad].TimeTask_Falge == false && !(TempChr & 0x01) && !(TempChr & waitFor)) {
        TempChr = I_CI522_IO_Read(ComIrqReg);
    }
    I_CI522_ClearBitMask(BitFramingReg, 0x80);
    if (SetTime.Task[TempReadCrad].TimeTask_Falge == true) {
        SetTime.CloseTask(TempReadCrad);
        EndRead();
        return MI_ERR;
    }
    SetTime.CloseTask(TempReadCrad);
    // I_CI522_IO_Read(ErrorReg);
    if (I_CI522_IO_Read(ErrorReg) & 0x1B) {
        EndRead();
        return MI_ERR;
    }
    status = MI_OK;
    if (TempChr & irqEn & 0x01) {
        status = MI_NOTAGERR;
    }

    if (Command == PCD_TRANSCEIVE) {
        TempChr = I_CI522_IO_Read(FIFOLevelReg);
        lastBits = I_CI522_IO_Read(ControlReg) & 0x07;
        if (pOutLenBit != NULL) {
            if (lastBits) {
                *pOutLenBit = (TempChr - 1) * 8 + lastBits;
            } else {
                *pOutLenBit = TempChr * 8;
            }
        }
        if (TempChr == 0) {
            TempChr = 1;
        }
        if (TempChr > MAXRLEN) {
            TempChr = MAXRLEN;
        }
        for (uint8_t i = 0; i < TempChr; i++) {
            pOutData[i] = I_CI522_IO_Read(FIFODataReg);
        }
    }
    EndRead();
    return status;
}

/********************************************************************
 * 函数功能：寻卡
 * 参数说明: req_code[IN]:寻卡方式
 *                0x52 = 寻感应区内所有符合14443A标准的卡
 *                0x26 = 寻未进入休眠状态的卡
 *          pTagType[OUT]：卡片类型代码
 *                0x4400 = Mifare_UltraLight
 *                0x0400 = Mifare_One(S50)
 *                0x0200 = Mifare_One(S70)
 *                0x0800 = Mifare_Pro(X)
 *                0x4403 = Mifare_DESFire
 * 返回值 : 成功返回MI_OK
 ********************************************************************/
char PCDRequest(unsigned char req_code, unsigned char * pTagType) {
    I_CI522_ClearBitMask(Status2Reg, 0x08);
    I_CI522_IO_Write(BitFramingReg, 0x07);
    I_CI522_SetBitMask(TxControlReg, 0x03);

    unsigned char ucComMF522Buf[MAXRLEN];
    unsigned int unLen = 0;
    ucComMF522Buf[0] = req_code;
    char status = PCDComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen);
    if ((status == MI_OK) && (unLen == 0x10)) {
        *pTagType = ucComMF522Buf[0];
        *(pTagType + 1) = ucComMF522Buf[1];
    } else {
        status = MI_ERR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PCDAnticoll(unsigned char * pSnr, unsigned char anticollision_level) {
    I_CI522_ClearBitMask(Status2Reg, 0x08);
    I_CI522_IO_Write(BitFramingReg, 0x00);
    I_CI522_ClearBitMask(CollReg, 0x80);

    unsigned char ucComMF522Buf[MAXRLEN];
    ucComMF522Buf[0] = anticollision_level;
    ucComMF522Buf[1] = 0x20;

    if (PCDComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, NULL) != MI_OK) {
        I_CI522_SetBitMask(CollReg, 0x80);
        return MI_ERR;
    }

    unsigned char status = MI_OK;
    unsigned char snr_check = 0;
    for (uint8_t i = 0; i < 5; i++) {
        if (i < 4) {
            *(pSnr + i) = ucComMF522Buf[i];
            snr_check ^= ucComMF522Buf[i];
        } else {
            if (snr_check != ucComMF522Buf[i]) {
                status = MI_ERR;
            }
        }
    }
    I_CI522_SetBitMask(CollReg, 0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
////////////////////////////////////////////////////////////////////
char PCDSelect(unsigned char * pSnr, unsigned char * sak, uint8_t ANTICO_Num) {
    unsigned char ucComMF522Buf[MAXRLEN];
    ucComMF522Buf[0] = ANTICO_Num;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (uint8_t i = 0; i < 4; i++) {
        ucComMF522Buf[i + 2] = *(pSnr + i);
        ucComMF522Buf[6] ^= *(pSnr + i);
    }
    CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);
    I_CI522_ClearBitMask(Status2Reg, 0x08);
    unsigned int unLen;
    char status = PCDComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &unLen);
    if ((status == MI_OK) && (unLen == 0x18)) {
        *sak = ucComMF522Buf[0];
        status = MI_OK;
    } else {
        status = MI_ERR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PCDHalt(void) {
    unsigned char ucComMF522Buf[MAXRLEN];
    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);
    return PCDComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, NULL);
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////               
char PCDAuthState(unsigned char auth_mode, unsigned char addr, unsigned char * pKey, unsigned char * pSnr) {
    unsigned char ucComMF522Buf[MAXRLEN];
    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    memcpy(&ucComMF522Buf[2], pKey, 6);
    memcpy(&ucComMF522Buf[8], pSnr, 6);
    char status = PCDComMF522(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, NULL);
    if ((status != MI_OK) || (!(I_CI522_IO_Read(Status2Reg) & 0x08))) {
        status = MI_ERR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PCDRead(unsigned char addr, unsigned char * pData) {
    unsigned char ucComMF522Buf[MAXRLEN];
    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);
    unsigned int unLen = 0;
    char status = PCDComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
    if ((status == MI_OK) && (unLen == 0x90)) {
        memcpy(pData, ucComMF522Buf, 16);
    } else {
        status = MI_ERR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
char PCDWrite(unsigned char addr, unsigned char * pData) {
    unsigned char ucComMF522Buf[MAXRLEN];
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);
    unsigned int unLen = 0;
    char status = PCDComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
        status = MI_ERR;
    }
    if (status == MI_OK) {
        memcpy(ucComMF522Buf, pData, 16);
        CalulateCRC(ucComMF522Buf, 16, &ucComMF522Buf[16]);

        status = PCDComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, &unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
            status = MI_ERR;
        }
    }
    return status;
}
// read UserCard
bool ReadCardData(UserCard * UserCardData) {
    newString(TempBuf, 16);
    if (PCDRead(4, (unsigned char *)TempBuf.Name._char) != MI_OK) {
        return false;
    }
    (*UserCardData).ValidNum = U8_Connect_U8(TempBuf.Name._char[0], TempBuf.Name._char[1]);
    (*UserCardData).CardType = TempBuf.Name._char[2];
    memcpy((*UserCardData).MeterID, &TempBuf.Name._char[3], 4);
    if (PCDRead(5, (unsigned char *)TempBuf.Name._char) != MI_OK) {
        return false;
    }
    memcpy((*UserCardData).RTC_Time_Set, &TempBuf.Name._char[0], 6);
    memcpy((*UserCardData).LimitTime, &TempBuf.Name._char[6], 3);
    (*UserCardData).LimitTimeDegree = U8_Connect_U8(TempBuf.Name._char[0x09], TempBuf.Name._char[0x0A]);
    (*UserCardData).ModeCode = U8_Connect_U8(TempBuf.Name._char[0x0B], TempBuf.Name._char[0x0C]);
    return true;
}
// write UserCard
bool WriteCardData(UserCard * UserCardData) {
    newString(TempBuf, 16);
    TempBuf.Name._char[0] = (((*UserCardData).ValidNum >> 8) & 0x00FF);
    TempBuf.Name._char[1] = (((*UserCardData).ValidNum >> 0) & 0x00FF);
    TempBuf.Name._char[2] = (*UserCardData).CardType;
    memcpy(&TempBuf.Name._char[3], (*UserCardData).MeterID, 4);
    if (PCDWrite(4, (unsigned char *)TempBuf.Name._char) != MI_OK) {
        return false;
    }
    memcpy(&TempBuf.Name._char[0], (*UserCardData).RTC_Time_Set, 6);
    memcpy(&TempBuf.Name._char[6], (*UserCardData).LimitTime, 3);
    TempBuf.Name._char[0x09] = (((*UserCardData).LimitTimeDegree >> 8) & 0x00FF);
    TempBuf.Name._char[0x0A] = (((*UserCardData).LimitTimeDegree >> 0) & 0x00FF);
    TempBuf.Name._char[0x0B] = (((*UserCardData).ModeCode >> 8) & 0x00FF);
    TempBuf.Name._char[0x0C] = (((*UserCardData).ModeCode >> 0) & 0x00FF);
    if (PCDWrite(5, (unsigned char *)TempBuf.Name._char) != MI_OK) {
        return false;
    }
    return true;
}

bool PCD_CI522_TypeA_rw_block(bool (*DoneFun)(UserCard * UserCardData)) {
    unsigned char ATQA[2];
    //request 寻卡 //寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
    I_CI522_IO_Write(RFCfgReg, RFCfgReg_Val); //复位接收增益
    if (PCDRequest(PICC_REQIDL, ATQA) != MI_OK) {
        I_CI522_IO_Write(RFCfgReg, 0x48);
        if (PCDRequest(PICC_REQIDL, ATQA) != MI_OK) {
            I_CI522_IO_Write(RFCfgReg, 0x58);
            if (PCDRequest(PICC_REQIDL, ATQA) != MI_OK) {
                return false;
            }
        }
    }
    unsigned char UID[12];
    //Anticoll 冲突检测
    if (PCDAnticoll(UID, PICC_ANTICOLL1) != MI_OK) {
        SetErrerCode(CODE_ERR_CARD_FAULT, true);
        return false;
    }
    unsigned char SAK = 0;
    //Select 选卡
    if (PCDSelect(UID, &SAK, PICC_ANTICOLL1) != MI_OK) {
        SetErrerCode(CODE_ERR_CARD_FAULT, true);
        return false;
    }
    // Authenticate 验证 A 密码
    unsigned char DefaultKeyABuf[10] = {0xAB, 0x12, 0xAB, 0x34, 0xAB, 0x56, 0x7F, 0x07, 0x88, 0x69}; // 注意：7F 07 88 69
    // unsigned char DefaultKeyABuf[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    if (PCDAuthState(PICC_AUTHENT1A, 4, DefaultKeyABuf, UID) != MI_OK) {
        SetErrerCode(CODE_ERR_CARD_FAULT, true);
        return false;
    }
    UserCard M1_Card_Data = {
        .ValidNum = 0,
        .CardType = 0,
        .MeterID = {0},
        .RTC_Time_Set = {0},
        .LimitTime = {0},
        .LimitTimeDegree = 0,
        .ModeCode = 0,
    };
    // 读BLOCK原始数据
    if (ReadCardData(&M1_Card_Data) == false) {
        SetErrerCode(CODE_ERR_CARD_FAULT, true);
        return false;
    }
    if (M1_Card_Data.ValidNum <= 0) {
        SetErrerCode(CODE_ERR_CARD_FAULT, true);
        return false;
    }
    SetErrerCode(CODE_ERR_CARD_FAULT, false);
    if (!(M1_Card_Data.MeterID[0] == 0xFF) && (M1_Card_Data.MeterID[1] == 0xFF) && (M1_Card_Data.MeterID[2] == 0xFF) && (M1_Card_Data.MeterID[3] == 0xFF)) {
        // 修改 UserCardData
        M1_Card_Data.ValidNum = (M1_Card_Data.ValidNum > 0 ? M1_Card_Data.ValidNum - 1 : 0);
    }
    if (M1_Card_Data.ValidNum == 0) {
        memset(M1_Card_Data.LimitTime, 0, 3);
        M1_Card_Data.LimitTime[0] = 0x00;
        M1_Card_Data.LimitTime[1] = 0x01;
        M1_Card_Data.LimitTime[2] = 0x01;
        M1_Card_Data.LimitTimeDegree = 0;
    }
    // 写BLOCK 写入新的数据
    if (WriteCardData(&M1_Card_Data) == false) {
        SetErrerCode(CODE_ERR_CARD_FAULT, true);
        return false;
    }
    SetErrerCode(CODE_ERR_CARD_FAULT, false);
    bool status = DoneFun(&M1_Card_Data);
    // //Halt
    // if (PCDHalt() != MI_OK) {
    //     return false;
    // }
    return status;
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

/***********************************************
 * 函数名：PCDReset
 * 描述  ：复位RC522
 * 输入  ：无
 * 返回  : 无
 * 调用  ：外部调用
 **********************************************/
void PCDReset(void) {
//	hard reset
//	HAL_GPIO_WritePin(S52_NRSTPD_GPIO_Port,S52_NRSTPD_Pin,GPIO_PIN_RESET);
//	FL_DelayUs(100);
//	HAL_GPIO_WritePin(S52_NRSTPD_GPIO_Port,S52_NRSTPD_Pin,GPIO_PIN_RESET);
//	FL_DelayUs(100);	
    I_CI522_IO_Write(CommandReg, 0x0f);			      //向CommandReg 写入 0x0f	作用是使RC522复位
    while (I_CI522_IO_Read(CommandReg) & 0x10);	  //Powerdown位为0时，表示RC522已准备好
    FL_DelayUs(100);
}

// void PCDPowerdown(void) {
//     GPIO_ResetBits(GPIOA, GPIO_Pin_2);
//     FL_DelayMs(5);
//     GPIO_SetBits(GPIOA, GPIO_Pin_2);
//     FL_DelayMs(5);
// }
//ci522_interfaces
// void I_CI522_SiModifyReg(unsigned char RegAddr, unsigned char ModifyVal, unsigned char MaskByte) {
//     unsigned char RegVal;
//     RegVal = I_CI522_IO_Read(RegAddr);
//     if (ModifyVal) {
//         RegVal |= MaskByte;
//     } else {
//         RegVal &= (~MaskByte);
//     }
//     I_CI522_IO_Write(RegAddr, RegVal);
// }
