#include "SI522A_App.h"
#include <stdlib.h>
#include <stdio.h>
#include "SI522A_interface.h"
#include "string.h"
#include "main.h"

//***********************************//修改新增内容
UserCard CardData = {0};
extern uint8_t PCD_IRQ_flagA;
unsigned char ACDConfigRegK_Val;
unsigned char ACDConfigRegC_Val;

/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void) {
    unsigned char i;
    i = I_SI522A_IO_Read(TxControlReg);
    if (!(i & 0x03)) {
        I_SI522A_SetBitMask(TxControlReg, 0x03);
    }
}

/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void) {
    I_SI522A_ClearBitMask(TxControlReg, 0x03);
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(char * pIndata, unsigned char len, char * pOutData) {
    unsigned char i, n;
    I_SI522A_ClearBitMask(DivIrqReg, 0x04);
    I_SI522A_IO_Write(CommandReg, PCD_IDLE);
    I_SI522A_SetBitMask(FIFOLevelReg, 0x80);
    for (i = 0; i < len; i++) {
        I_SI522A_IO_Write(FIFODataReg, *(pIndata + i));
    }
    I_SI522A_IO_Write(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do {
        n = I_SI522A_IO_Read(DivIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x04));
    pOutData[0] = I_SI522A_IO_Read(CRCResultRegL);
    pOutData[1] = I_SI522A_IO_Read(CRCResultRegH);
}

unsigned char aaa = 0;

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
//status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
char PcdComMF522(unsigned char Command,
    char * pInData,
    unsigned char InLenByte,
    char * pOutData,
    unsigned int * pOutLenBit) {
    char status = MI_ERR;
    unsigned char irqEn = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
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

    //I_SI522A_IO_Write(ComIEnReg,irqEn|0x80);
    I_SI522A_ClearBitMask(ComIrqReg, 0x80);
    I_SI522A_IO_Write(CommandReg, PCD_IDLE);
    I_SI522A_SetBitMask(FIFOLevelReg, 0x80);

    for (i = 0; i < InLenByte; i++) {
        I_SI522A_IO_Write(FIFODataReg, pInData[i]);
    }
    I_SI522A_IO_Write(CommandReg, Command);

    if (Command == PCD_TRANSCEIVE) {
        I_SI522A_SetBitMask(BitFramingReg, 0x80);
    }

    //i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    i = 2000;
    do {
        n = I_SI522A_IO_Read(ComIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));
    I_SI522A_ClearBitMask(BitFramingReg, 0x80);

    if (i != 0) {
        aaa = I_SI522A_IO_Read(ErrorReg);

        if (!(I_SI522A_IO_Read(ErrorReg) & 0x1B)) {
            status = MI_OK;
            if (n & irqEn & 0x01) {
                status = MI_NOTAGERR;
            }
            if (Command == PCD_TRANSCEIVE) {
                n = I_SI522A_IO_Read(FIFOLevelReg);
                lastBits = I_SI522A_IO_Read(ControlReg) & 0x07;
                if (lastBits) {
                    *pOutLenBit = (n - 1) * 8 + lastBits;
                } else {
                    *pOutLenBit = n * 8;
                }
                if (n == 0) {
                    n = 1;
                }
                if (n > MAXRLEN) {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++) {
                    pOutData[i] = I_SI522A_IO_Read(FIFODataReg);
                }
            }
        } else {
            status = MI_ERR;
        }

    }

    I_SI522A_SetBitMask(ControlReg, 0x80);           // stop timer now
    I_SI522A_IO_Write(CommandReg, PCD_IDLE);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code, unsigned char * pTagType) {
    char status;
    unsigned int unLen;
    strnew_malloc(ucComMF522Buf, MAXRLEN);

    I_SI522A_ClearBitMask(Status2Reg, 0x08);
    I_SI522A_IO_Write(BitFramingReg, 0x07);
    I_SI522A_SetBitMask(TxControlReg, 0x03);

    ucComMF522Buf.Name._char[0] = req_code;

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf.Name._char, 1, ucComMF522Buf.Name._char, &unLen);
    if ((status == MI_OK) && (unLen == 0x10)) {
        *pTagType = ucComMF522Buf.Name._char[0];
        *(pTagType + 1) = ucComMF522Buf.Name._char[1];
    } else {
        status = MI_ERR;
    }
    free(ucComMF522Buf.Name._char);
    return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(unsigned char * pSnr, unsigned char anticollision_level) {
    char status;
    unsigned char i, snr_check = 0;
    unsigned int unLen;
    strnew_malloc(ucComMF522Buf, MAXRLEN);
    I_SI522A_ClearBitMask(Status2Reg, 0x08);
    I_SI522A_IO_Write(BitFramingReg, 0x00);
    I_SI522A_ClearBitMask(CollReg, 0x80);
    ucComMF522Buf.Name._char[0] = anticollision_level;
    ucComMF522Buf.Name._char[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf.Name._char, 2, ucComMF522Buf.Name._char, &unLen);

    if (status == MI_OK) {
        for (i = 0; i < 4; i++) {
            *(pSnr + i) = ucComMF522Buf.Name._char[i];
            snr_check ^= ucComMF522Buf.Name._char[i];
        }
        if (snr_check != ucComMF522Buf.Name._char[i]) {
            status = MI_ERR;
        }
    }

    I_SI522A_SetBitMask(CollReg, 0x80);
    free(ucComMF522Buf.Name._char);
    return status;
}


/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char * pSnr, unsigned char * sak) {
    char status;
    unsigned char i;
    unsigned int unLen;
    strnew_malloc(ucComMF522Buf, MAXRLEN);
    ucComMF522Buf.Name._char[0] = PICC_ANTICOLL1;
    ucComMF522Buf.Name._char[1] = 0x70;
    ucComMF522Buf.Name._char[6] = 0;
    for (i = 0; i < 4; i++) {
        ucComMF522Buf.Name._char[i + 2] = *(pSnr + i);
        ucComMF522Buf.Name._char[6] ^= *(pSnr + i);
    }
    CalulateCRC(ucComMF522Buf.Name._char, 7, &ucComMF522Buf.Name._char[7]);
    I_SI522A_ClearBitMask(Status2Reg, 0x08);
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf.Name._char, 9, ucComMF522Buf.Name._char, &unLen);
    if ((status == MI_OK) && (unLen == 0x18)) {
        *sak = ucComMF522Buf.Name._char[0];
        status = MI_OK;
    } else {
        status = MI_ERR;
    }
    free(ucComMF522Buf.Name._char);
    return status;
}

char PcdSelect1(unsigned char * pSnr, unsigned char * sak) {
    char status;
    unsigned char i;
    unsigned int unLen;
    strnew_malloc(ucComMF522Buf, MAXRLEN);

    ucComMF522Buf.Name._char[0] = PICC_ANTICOLL1;
    ucComMF522Buf.Name._char[1] = 0x70;
    ucComMF522Buf.Name._char[6] = 0;
    for (i = 0; i < 4; i++) {
        ucComMF522Buf.Name._char[i + 2] = *(pSnr + i);
        ucComMF522Buf.Name._char[6] ^= *(pSnr + i);
    }
    CalulateCRC(ucComMF522Buf.Name._char, 7, &ucComMF522Buf.Name._char[7]);
    I_SI522A_ClearBitMask(Status2Reg, 0x08);
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf.Name._char, 9, ucComMF522Buf.Name._char, &unLen);
    if ((status == MI_OK) && (unLen == 0x18)) {
        *sak = ucComMF522Buf.Name._char[0];
        status = MI_OK;
    } else {
        status = MI_ERR;
    }
    free(ucComMF522Buf.Name._char);
    return status;
}

char PcdSelect2(unsigned char * pSnr, unsigned char * sak) {
    char status;
    unsigned char i;
    unsigned int unLen;
    strnew_malloc(ucComMF522Buf, MAXRLEN);

    ucComMF522Buf.Name._char[0] = PICC_ANTICOLL2;
    ucComMF522Buf.Name._char[1] = 0x70;
    ucComMF522Buf.Name._char[6] = 0;
    for (i = 0; i < 4; i++) {
        ucComMF522Buf.Name._char[i + 2] = *(pSnr + i);
        ucComMF522Buf.Name._char[6] ^= *(pSnr + i);
    }
    CalulateCRC(ucComMF522Buf.Name._char, 7, &ucComMF522Buf.Name._char[7]);
    I_SI522A_ClearBitMask(Status2Reg, 0x08);
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf.Name._char, 9, ucComMF522Buf.Name._char, &unLen);
    if ((status == MI_OK) && (unLen == 0x18)) {
        *sak = ucComMF522Buf.Name._char[0];
        status = MI_OK;
    } else {
        status = MI_ERR;
    }
    free(ucComMF522Buf.Name._char);
    return status;
}

char PcdSelect3(unsigned char * pSnr, unsigned char * sak) {
    char status;
    unsigned char i;
    unsigned int unLen;
    strnew_malloc(ucComMF522Buf, MAXRLEN);

    ucComMF522Buf.Name._char[0] = PICC_ANTICOLL2;
    ucComMF522Buf.Name._char[1] = 0x70;
    ucComMF522Buf.Name._char[6] = 0;
    for (i = 0; i < 4; i++) {
        ucComMF522Buf.Name._char[i + 2] = *(pSnr + i);
        ucComMF522Buf.Name._char[6] ^= *(pSnr + i);
    }
    CalulateCRC(ucComMF522Buf.Name._char, 7, &ucComMF522Buf.Name._char[7]);
    I_SI522A_ClearBitMask(Status2Reg, 0x08);
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf.Name._char, 9, ucComMF522Buf.Name._char, &unLen);
    if ((status == MI_OK) && (unLen == 0x18)) {
        *sak = ucComMF522Buf.Name._char[0];
        status = MI_OK;
    } else {
        status = MI_ERR;
    }
    free(ucComMF522Buf.Name._char);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void) {
    char status;
    unsigned int unLen;
    strnew_malloc(ucComMF522Buf, MAXRLEN);
    ucComMF522Buf.Name._char[0] = PICC_HALT;
    ucComMF522Buf.Name._char[1] = 0;
    CalulateCRC(ucComMF522Buf.Name._char, 2, &ucComMF522Buf.Name._char[2]);
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf.Name._char, 4, ucComMF522Buf.Name._char, &unLen);
    free(ucComMF522Buf.Name._char);
    return status;
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
char PcdAuthState(unsigned char auth_mode, unsigned char addr, unsigned char * pKey, unsigned char * pSnr) {
    char status;
    unsigned int unLen;
    strnew_malloc(ucComMF522Buf, MAXRLEN);
    ucComMF522Buf.Name._char[0] = auth_mode;
    ucComMF522Buf.Name._char[1] = addr;
    memcpy(&ucComMF522Buf.Name._char[2], pKey, 6);
    memcpy(&ucComMF522Buf.Name._char[8], pSnr, 6);
    status = PcdComMF522(PCD_AUTHENT, ucComMF522Buf.Name._char, 12, ucComMF522Buf.Name._char, &unLen);
    if ((status != MI_OK) || (!(I_SI522A_IO_Read(Status2Reg) & 0x08))) {
        status = MI_ERR;
    }
    free(ucComMF522Buf.Name._char);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(unsigned char addr, unsigned char * pData, int DataLen) {
    char status;
    unsigned int unLen;
    strnew_malloc(ucComMF522Buf, DataLen);
    ucComMF522Buf.Name._char[0] = PICC_READ;
    ucComMF522Buf.Name._char[1] = addr;
    CalulateCRC(ucComMF522Buf.Name._char, 2, &ucComMF522Buf.Name._char[2]);
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf.Name._char, 4, ucComMF522Buf.Name._char, &unLen);
    if ((status == MI_OK) && (unLen == 0x90)) {
        memcpy(pData, ucComMF522Buf.Name._char, DataLen);
    } else {
        status = MI_ERR;
    }
    free(ucComMF522Buf.Name._char);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(unsigned char addr, unsigned char * pData, int DataLen) {
    char status;
    unsigned int unLen;
    strnew_malloc(ucComMF522Buf, DataLen);
    ucComMF522Buf.Name._char[0] = PICC_WRITE;
    ucComMF522Buf.Name._char[1] = addr;
    CalulateCRC(ucComMF522Buf.Name._char, 2, &ucComMF522Buf.Name._char[2]);
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf.Name._char, 4, ucComMF522Buf.Name._char, &unLen);
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf.Name._char[0] & 0x0F) != 0x0A)) {
        status = MI_ERR;
    }
    if (status == MI_OK) {
        memcpy(ucComMF522Buf.Name._char, pData, DataLen);
        CalulateCRC(ucComMF522Buf.Name._char, DataLen, &ucComMF522Buf.Name._char[16]);

        status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf.Name._char, 18, ucComMF522Buf.Name._char, &unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf.Name._char[0] & 0x0F) != 0x0A)) {
            status = MI_ERR;
        }
    }
    free(ucComMF522Buf.Name._char);
    return status;
}

void PCD_SI522A_TypeA_Init(void) {
    I_SI522A_ClearBitMask(Status2Reg, 0x08);
    // Reset baud rates
    I_SI522A_IO_Write(TxModeReg, 0x00);
    I_SI522A_IO_Write(RxModeReg, 0x00);
    // Reset ModWidthReg
    I_SI522A_IO_Write(ModWidthReg, 0x26);
    // RxGain:110,43dB by default;
    I_SI522A_IO_Write(RFCfgReg, RFCfgReg_Val);
    // When communicating with a PICC we need a timeout if something goes wrong.
    // f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
    // TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
    I_SI522A_IO_Write(TModeReg, 0x80);// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
    I_SI522A_IO_Write(TPrescalerReg, 0xa9);// TPreScaler = TModeReg[3..0]:TPrescalerReg
    I_SI522A_IO_Write(TReloadRegH, 0x03); // Reload timer 
    I_SI522A_IO_Write(TReloadRegL, 0xe8); // Reload timer 
    I_SI522A_IO_Write(TxASKReg, 0x40);	// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
    I_SI522A_IO_Write(ModeReg, 0x3D);	// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
    I_SI522A_IO_Write(CommandReg, 0x00);  // Turn on the analog part of receiver   
    PcdAntennaOn();
}

char PCD_SI522A_TypeA_GetUID(void) {
    unsigned char ATQA[2] = {0};
    unsigned char UID[12] = {0};
    unsigned char SAK = 0;
    unsigned char UID_complate1 = 0;
    unsigned char UID_complate2 = 0;

    // printf("\r\nTest_Si522A_GetUID");
    I_SI522A_IO_Write(RFCfgReg, RFCfgReg_Val); //复位接收增益
    //寻卡//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
    if (PcdRequest(PICC_REQIDL, ATQA) != MI_OK) {
        I_SI522A_IO_Write(RFCfgReg, 0x48);
        if (PcdRequest(PICC_REQIDL, ATQA) != MI_OK) {
            I_SI522A_IO_Write(RFCfgReg, 0x58);
            if (PcdRequest(PICC_REQIDL, ATQA) != MI_OK) {
                // printf("\r\nRequest:fail");
                return 1;
            } else {
                // printf("\r\nRequest1:ok  ATQA:%02x %02x", ATQA[0], ATQA[1]);
            }
        } else {
            // printf("\r\nRequest2:ok  ATQA:%02x %02x", ATQA[0], ATQA[1]);
        }
    } else {
        // printf("\r\nRequest3:ok  ATQA:%02x %02x", ATQA[0], ATQA[1]);
    }


    //UID长度=4
    //Anticoll 冲突检测 level1
    if (PcdAnticoll(UID, PICC_ANTICOLL1) != MI_OK) {
        // printf("\r\nAnticoll1:fail");
        return 1;
    } else {
        if (PcdSelect1(UID, &SAK) != MI_OK) {
            // printf("\r\nSelect1:fail");
            return 1;
        } else {
            // printf("\r\nSelect1:ok  SAK1:%02x", SAK);
            if (SAK & 0x04) {
                UID_complate1 = 0;
                //UID长度=7
                if (UID_complate1 == 0) {
                    //Anticoll 冲突检测 level2
                    if (PcdAnticoll(UID + 4, PICC_ANTICOLL2) != MI_OK) {
                        // printf("\r\nAnticoll2:fail");
                        return 1;
                    } else {
                        if (PcdSelect2(UID + 4, &SAK) != MI_OK) {
                            // printf("\r\nSelect2:fail");
                            return 1;
                        } else {
                            // printf("\r\nSelect2:ok  SAK2:%02x", SAK);
                            if (SAK & 0x04) {
                                UID_complate2 = 0;

                                //UID长度=10
                                if (UID_complate2 == 0) {
                                    //Anticoll 冲突检测 level3
                                    if (PcdAnticoll(UID + 8, PICC_ANTICOLL3) != MI_OK) {
                                        // printf("\r\nAnticoll3:fail");
                                        return 1;
                                    } else {
                                        if (PcdSelect3(UID + 8, &SAK) != MI_OK) {
                                            // printf("\r\nSelect3:fail");
                                            return 1;
                                        } else {
                                            // printf("\r\nSelect3:ok  SAK3:%02x", SAK);
                                            if (SAK & 0x04) {
//												UID_complate3 = 0;
                                            } else {
    //											UID_complate3 = 1;                   
                                                // printf("\r\nAnticoll3:ok  UID:%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", UID[1], UID[2], UID[3], UID[5], UID[6], UID[7], UID[8], UID[9], UID[10], UID[11]);
                                            }
                                        }
                                    }
                                }
                            } else {
                                UID_complate2 = 1;
                                // printf("\r\nAnticoll2:ok  UID:%02x %02x %02x %02x %02x %02x %02x", UID[1], UID[2], UID[3], UID[4], UID[5], UID[6], UID[7]);
                            }
                        }
                    }
                }
            } else {
                UID_complate1 = 1;
                // printf("\r\nAnticoll1:ok  UID:%02x %02x %02x %02x", UID[0], UID[1], UID[2], UID[3]);
            }
        }
    }
    //Halt
    if (PcdHalt() != MI_OK) {
        // printf("\r\nHalt:fail");
        return 1;
    } else {
        // printf("\r\nHalt:ok");
    }

    FL_DelayUs(100);
    return 0;
}

bool PCD_SI522A_TypeA_rw_block(bool (*DoneFun)(UserCard UserCardData)) {
    unsigned char ATQA[2];
    unsigned char UID[12];
    unsigned char SAK = 0;
    unsigned char DefaultKeyABuf[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    //unsigned char DefaultKeyABuf[10] = { 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xFF, 0xFF, 0xFF, 0xFF };
    UserCard M1_Card_Data = {
        .SetDegree = 1000,
        .CmdFlag = 0,
        .MeterID = {0x12, 0x34, 0x56, 0x78},
        .SetLimitTime = {0x99, 0x01, 0x01, 0x14, 0x58, 0x00},
        .SetRTCTime = {0x99, 0x01, 0x01, 0x14, 0x58, 0x00},
    };
        //request 寻卡 //寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
    if (PcdRequest(PICC_REQIDL, ATQA) != MI_OK) {
        return false;
    } else {
        // printf("\r\nRequest:ok  ATQA:%02x %02x", ATQA[0], ATQA[1]);
    }
    //Anticoll 冲突检测
    if (PcdAnticoll(UID, PICC_ANTICOLL1) != MI_OK) {
        SetErrerCode(CODE_ERR_CARD_FAULT, true);
        return false;
    } else {
        // printf("\r\nAnticoll:ok  UID:%02x %02x %02x %02x", UID[0], UID[1], UID[2], UID[3]);
    }
    //Select 选卡
    if (PcdSelect1(UID, &SAK) != MI_OK) {
        SetErrerCode(CODE_ERR_CARD_FAULT, true);
        return false;
    } else {
        // printf("\r\nSelect:ok  SAK:%02x", SAK);
    }
    //Authenticate 验证密码
    if (PcdAuthState(PICC_AUTHENT1A, 4, DefaultKeyABuf, UID) != MI_OK) {
        SetErrerCode(CODE_ERR_CARD_FAULT, true);
        return false;
    } else {
        // printf("\r\nAuthenticate:ok");
    }
    bool status = false;
    //读BLOCK原始数据
    if (PcdRead(4, (unsigned char *)&M1_Card_Data, sizeof(M1_Card_Data)) != MI_OK) {
        SetErrerCode(CODE_ERR_CARD_FAULT, true);
        return false;
    } else {
        SetErrerCode(CODE_ERR_CARD_FAULT, false);
        status = DoneFun(M1_Card_Data);
    }

    // //写BLOCK 写入新的数据
    // if (PcdWrite(4, (unsigned char *)&M1_Card_Data, sizeof(M1_Card_Data)) != MI_OK) {
    //     // printf("\r\nPcdWrite:fail");
    //     return 1;
    // } else {
    //     // printf("\r\nPcdWrite:ok  ");
    //     for (unsigned char i = 0; i < 16; i++) {
    //         // printf(" %02x", CardWriteBuf[i]);
    //     }
    // }

    // //读BLOCK 读出新写入的数据
    // if (PcdRead(4, (unsigned char *)&M1_Card_Data, sizeof(M1_Card_Data)) != MI_OK) {
    //     // printf("\r\nPcdRead:fail");
    //     return 1;
    // } else {
    //     // printf("\r\nPcdRead:ok  ");
    //     for (unsigned char i = 0; i < 16; i++) {
    //         // printf(" %02x", CardReadBuf[i]);
    //     }
    // }

    PcdHalt();
    return status;
}

//***********************************//修改新增内容

/*
 * 函数名：PcdReset
 * 描述  ：复位RC522
 * 输入  ：无
 * 返回  : 无
 * 调用  ：外部调用
 */
void PcdReset(void) {
    //hard reset
    GPIO_SET_L(GPIOB, FL_GPIO_PIN_12); //	HAL_GPIO_WritePin(S52_NRSTPD_GPIO_Port,S52_NRSTPD_Pin,GPIO_PIN_RESET);
    FL_DelayUs(100);
    GPIO_SET_H(GPIOB, FL_GPIO_PIN_12); //	HAL_GPIO_WritePin(S52_NRSTPD_GPIO_Port,S52_NRSTPD_Pin,GPIO_PIN_SET);
    FL_DelayUs(100);

    I_SI522A_IO_Write(CommandReg, 0x0f);			//向CommandReg 写入 0x0f	作用是使RC522复位
    while (I_SI522A_IO_Read(CommandReg) & 0x10);	//Powerdown位为0时，表示RC522已准备好
    FL_DelayUs(100);
}

void Pcd_Hard_Reset(void) {
    GPIO_SET_L(GPIOB, FL_GPIO_PIN_12);  //	GPIO_ResetBits( GPIOA , GPIO_Pin_2 );    // NPDREST 引脚，MCU需要设置上拉输出
    FL_DelayMs(2);
    GPIO_SET_H(GPIOB, FL_GPIO_PIN_12);  //GPIO_SetBits( GPIOA , GPIO_Pin_2 );
    FL_DelayMs(2);
}


void I_SI522A_ClearBitMask(unsigned char reg, unsigned char mask) {
    char tmp = 0x00;
    tmp = I_SI522A_IO_Read(reg);
    I_SI522A_IO_Write(reg, tmp & ~mask);  // clear bit mask
}

void I_SI522A_SetBitMask(unsigned char reg, unsigned char mask) {
    char tmp = 0x00;
    tmp = I_SI522A_IO_Read(reg);
    I_SI522A_IO_Write(reg, tmp | mask);  // set bit mask
}

void I_SI522A_SiModifyReg(unsigned char RegAddr, unsigned char ModifyVal, unsigned char MaskByte) {
    unsigned char RegVal;
    RegVal = I_SI522A_IO_Read(RegAddr);
    if (ModifyVal) {
        RegVal |= MaskByte;
    } else {
        RegVal &= (~MaskByte);
    }
    I_SI522A_IO_Write(RegAddr, RegVal);
}

//
void ACD_init_Fun(void) {
    PCD_SI522A_TypeA_Init();	//Reader模式的初始化
    PCD_ACD_AutoCalc(); //自动获取阈值
    PCD_ACD_Init();   //初始化ACD配置寄存器，并且进入ACD模式
}

void ACD_Fun(void) {
//	EXTI->IMR |= 0x00000008;	// Enable external interrupt
//	PCD_IRQ_flagA = 0;	//clear IRQ flag
//	ledGreenShining();	// LED indicator
//	
//	while(1)
//	{	
//		if(PCD_IRQ_flagA)
//		{
//			printf("\r\n\r\n\r\n PCD_IRQ_flagA");
//			EXTI->IMR &= 0xFFFFFFF7;		// Disable external interrupt			

//			switch( PCD_IRQ() )
//			{
//				case 0:	//Other_IRQ 			
//					//printf("Other IRQ Occur\r\n");
//					PCD_SI522A_TypeA_GetUID();
//					//PcdReset();			//软复位				
//					Pcd_Hard_Reset();			//硬复位
//					PCD_SI522A_TypeA_Init();
//					PCD_ACD_Init();
//					break;
//						
//				case 1:	//ACD_IRQ
//					I_SI522A_SiModifyReg(0x01, 0, 0x20);	// Turn on the analog part of receiver 			
//					PCD_SI522A_TypeA_GetUID();
//					//I_SI522A_IO_Write(ComIEnReg, 0x80);		//复位02寄存器,在读卡函数中被改动
//					I_SI522A_IO_Write(CommandReg, 0xb0);	 	//进入软掉电,重新进入ACD（ALPPL）
//				break;
//				
//				case 2:	//ACDTIMER_IRQ			
//					printf("ACDTIMER_IRQ:Reconfigure the register \r\n");
//					//PcdReset();			//软复位
//					Pcd_Hard_Reset();		//硬复位
//					PCD_SI522A_TypeA_Init();
//					PCD_ACD_Init();
//					break;
//				
//			}		
//			
//			EXTI->IMR |= 0x00000008;		// Enable external interrupt
//			PCD_IRQ_flagA = 0;	
//		}
//		else
//		{
//			FL_DelayMs(500);
//		}
//	}
}

/********************************************************
*PCD_ACD_AutoCalc(void) 函数主要为了上电后，自动计算并选择ADC的基准电压和合适的增益放大。
*1.第一个VOCN数组是为了从最大基准电压开始轮询查，并将VCON值写入0F_K寄存器的bit[3:0],然后读取0F_G寄存器，计算100次的平均值，直到找到非0的最小值。
*2. 0F_K寄存器在第一步获取的VCON值基础上，或上TR增益控制位bit[6:5]，然后将换算的值写入0F_K寄存器，读取0F_G,获取最大非超7F阈值的增益控制位
*3. 将获取的VCON和TR，写入0F_G。
*4. 关于0F_K寄存器的定义请参考 《ACD初始化代码解析》文档
********************************************************/
void PCD_ACD_AutoCalc(void) {
    unsigned char temp;
    unsigned char temp_Compare = 0;
    unsigned char VCON_TR[8] = {0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};//acd灵敏度调节
    unsigned char TR_Compare[4] = {0x00, 0x00, 0x00, 0x00};
    ACDConfigRegC_Val = 0x7f;
    unsigned char	ACDConfigRegK_RealVal = 0;
    I_SI522A_IO_Write(TxControlReg, 0x83);	//打开天线
    I_SI522A_SetBitMask(CommandReg, 0x06);	//开启ADC_EXCUTE
    FL_DelayUs(200);

    for (int i = 7; i > 0; i--) {
        I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigK << 2) | 0x40);
        I_SI522A_IO_Write(ACDConfigReg, VCON_TR[i]);
        I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigG << 2) | 0x40);
        temp_Compare = I_SI522A_IO_Read(ACDConfigReg);
        for (int m = 0; m < 100; m++) {
            I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigG << 2) | 0x40);
            temp = I_SI522A_IO_Read(ACDConfigReg);

            if (temp == 0)
                break;  //处在接近的VCON值附近值，如果偶合出现0值，均有概率误触发，应舍弃该值。
            temp_Compare = (temp_Compare + temp) / 2;
            FL_DelayUs(100);
        }
        //比较当前值和所存值
        if (temp_Compare == 0 || temp_Compare == 0x7f) {
        } else {
            if (temp_Compare < ACDConfigRegC_Val) {
                ACDConfigRegC_Val = temp_Compare;
                ACDConfigRegK_Val = VCON_TR[i];
            }
        }
    }
    ACDConfigRegK_RealVal = ACDConfigRegK_Val;     //取得最接近的参考电压VCON
    for (int j = 0; j < 4; j++) {
        I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigK << 2) | 0x40);
        I_SI522A_IO_Write(ACDConfigReg, j * 32 + ACDConfigRegK_Val);
        I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigG << 2) | 0x40);
        temp_Compare = I_SI522A_IO_Read(ACDConfigReg);
        for (int n = 0; n < 100; n++) {
            I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigG << 2) | 0x40);
            temp = I_SI522A_IO_Read(ACDConfigReg);
            temp_Compare = (temp_Compare + temp) / 2;
            FL_DelayUs(100);
        }
        TR_Compare[j] = temp_Compare;
    }//再调TR的档位，将采集值填入TR_Compare[]

    //TR有四档可调，但是最大档的时候，电源有抖动，可能会导致ADC的值抖动较大，造成误触发
    for (int z = 0; z < 3; z++) {
        if (TR_Compare[z] == 0x7F) {

        } else {
            ACDConfigRegC_Val = TR_Compare[z];//最终选择的配置
            ACDConfigRegK_Val = ACDConfigRegK_RealVal + z * 32;
        }
    }//再选出一个非7f大值


    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigK << 2) | 0x40);
    // printf("\r\n ACDConfigRegK_Val:%02x ", ACDConfigRegK_Val);

    I_SI522A_SetBitMask(CommandReg, 0x06);		//关闭ADC_EXCUTE
}

void PCD_ACD_Init(void) {
    I_SI522A_IO_Write(DivIrqReg, 0x60);	//清中断，该处不清中断，进入ACD模式后会异常产生有卡中断。
    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigJ << 2) | 0x40);
    I_SI522A_IO_Write(ACDConfigReg, 0x55);	//Clear ACC_IRQ
    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigA << 2) | 0x40);          //设置轮询时间
    I_SI522A_IO_Write(ACDConfigReg, ACDConfigRegA_Val);
    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigB << 2) | 0x40);					//设置相对模式或者绝对模式
    I_SI522A_IO_Write(ACDConfigReg, ACDConfigRegB_Val);
    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigC << 2) | 0x40);					//设置无卡场强值
    I_SI522A_IO_Write(ACDConfigReg, ACDConfigRegC_Val);
    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigD << 2) | 0x40);					//设置灵敏度，一般建议为4，在调试时，可以适当降低验证该值，验证ACD功能
    I_SI522A_IO_Write(ACDConfigReg, ACDConfigRegD_Val);
    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigH << 2) | 0x40);					//设置看门狗定时器时间
    I_SI522A_IO_Write(ACDConfigReg, ACDConfigRegH_Val);
    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigI << 2) | 0x40);         //设置ARI功能，在天线场强打开前1us产生ARI电平控制触摸芯片Si12T的硬件屏蔽引脚SCT
    I_SI522A_IO_Write(ACDConfigReg, ACDConfigRegI_Val);
    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigK << 2) | 0x40);					//设置ADC的基准电压和放大增益
    I_SI522A_IO_Write(ACDConfigReg, ACDConfigRegK_Val);
    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigM << 2) | 0x40);					//设置监测ACD功能是否产生场强，意外产生可能导致读卡芯片复位或者寄存器丢失
    I_SI522A_IO_Write(ACDConfigReg, ACDConfigRegM_Val);
    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigO << 2) | 0x40);					//设置ACD模式下相关功能的标志位传导到IRQ引脚
    I_SI522A_IO_Write(ACDConfigReg, ACDConfigRegO_Val);
    I_SI522A_IO_Write(ComIEnReg, ComIEnReg_Val);												//ComIEnReg，DivIEnReg   设置IRQ选择上升沿或者下降沿
    I_SI522A_IO_Write(DivIEnReg, DivIEnReg_Val);
    I_SI522A_IO_Write(ACDConfigSelReg, (ACDConfigJ << 2) | 0x40);				//设置监测ACD功能下的重要寄存器的配置值，寄存器丢失后会立即产生中断
    I_SI522A_IO_Write(ACDConfigReg, ACDConfigRegJ_Val);								// 写非0x55的值即开启功能，写0x55清除使能停止功能。
    I_SI522A_IO_Write(CommandReg, 0xb0);	//进入ACD
}

char PCD_IRQ(void) {
    unsigned char status_SI522ACD_IRQ;
    unsigned char temp_SI522ACD_IRQ;

    temp_SI522ACD_IRQ = I_SI522A_IO_Read(DivIrqReg);

    //ACD中断
    if (temp_SI522ACD_IRQ & 0x40) {
        I_SI522A_IO_Write(DivIrqReg, 0x40);		//Clear ACDIRq
        status_SI522ACD_IRQ = 1;
        return status_SI522ACD_IRQ;
    }
    //ACD看门狗中断
    if (temp_SI522ACD_IRQ & 0x20) {
        I_SI522A_IO_Write(DivIrqReg, 0x20);		//Clear ACDTIMER_IRQ
        status_SI522ACD_IRQ = 2;
        return status_SI522ACD_IRQ;
    }
    I_SI522A_IO_Write(DivIrqReg, 0x40);		//Clear ACDIRq
    I_SI522A_IO_Write(DivIrqReg, 0x20);		//Clear ACDTIMER_IRQ
    I_SI522A_IO_Write(0x20, (0x0f << 2) | 0x40);
    I_SI522A_IO_Write(0x0f, 0x0a);	//Clear OSCMon_IRQ,RFLowDetect_IRQ
    I_SI522A_IO_Write(0x20, (0x09 << 2) | 0x40);
    I_SI522A_IO_Write(0x0f, 0x55);	//Clear ACC_IRQ

    return status_SI522ACD_IRQ = 0;

}


