#include "Define.h"
#include "LPUart_0_And_1_Lib.h"
#include "AT24CXXDataLoader.h"
#include "PT1000.h"

char TagToType[][2] = {
    {positiveHeat_TAG,Type_float},
    {negativeHeat_TAG,Type_float},
    {positiveFlow_TAG,Type_float},
    {negativeFlow_TAG,Type_float},
    {positiveSpeed_TAG,Type_float},
    {negativeSpeed_TAG,Type_float},
    {positiveTemperature_TAG,Type_float},
    {negativeTemperature_TAG,Type_float},
    {temperatureHigh_TAG,Type_float},
    {positivePressure_TAG,Type_float},
    {negativePressure_TAG,Type_float},
    {pressureHigh_TAG,Type_float},
    {devHigh_TAG,Type_float},
    {powerRate_TAG,Type_float},
    {voltage_TAG,Type_uint16_t},
    {electricity_TAG,Type_float},
    {workTime_TAG,Type_Duration},
    {nowTime_TAG,Type_Timestamp},
    {CCID_TAG,Type_HEXS},
    {IMEI_TAG,Type_HEXS},
    {CSQ_TAG,Type_int8_t},
    {RSRP_TAG,Type_int8_t},
    {RSRQ_TAG,Type_int8_t},
    {Moved_TAG,Type_int8_t},
    {SendClassObj_TAG,Type_Send_obj},
    {RoomTemperArray_TAG,Type_Array_obj},
    {upData_TAG,Type_ASCII},
    {SendCount_TAG,Type_uint16_t},
    {receiveConut_TAG,Type_uint16_t},
    {SOFT_VERSION_TAG,Type_int8_t},
    {NowTemper,Type_float},
    {MessageResFlag,Type_bin},
    {ValveRate,Type_float},
    {ValveFrequency,Type_float},
    {FlagOfRights,Type_uint16_t},
    {IPAndProt,Type_ASCII},
    {TemperSet_TAG,Type_float},
    {Timeblock_TAG,Type_Timeblock_obj},
    {MeterAndTargeID_TAG,Type_MeterAndTargeID_obj},
    {NowRunMode_TAG,Type_uint8_t},
    {NowPressure,Type_float},
    {CurrentRSSI,Type_uint8_t},
    {FrequencyPoint,Type_uint8_t},
    {LoarSendInterval,Type_uint32_t},
    {LoarSendFlag,Type_bin},
    {MeterType_TAG,Type_uint8_t},
    {ErrorCode,Type_uint16_t},
    {PD1000_R,Type_uint16_t},
};

const char SOFT_VERSION = 11; // 版本号
unsigned int UART_BAND = 115200;

// LongUART UART1Ddata;
ShortUART LPUART0Ddata, LPUART1Ddata;

unsigned int Now_DEV_Volt = 3600;
float Now_TemperOrPress = 0.0;

// 校验计算
unsigned char Cault_CS(char * Buffer, unsigned char start_num, unsigned char stop_num) {
    unsigned char temp, cs;
    cs = 0;
    for (temp = start_num; temp < stop_num; temp++) {
        cs = cs + *(Buffer + temp);
    }
    return cs;
}
int Add_HY_DataClass(strnew OutPutBuff, int startAddr, unsigned char DataTAG, unsigned char VIF) {
    // uint16_t TempNum = 0;
    int HeadStartAddr = startAddr;
    startAddr++;
    startAddr++;
    OutPutBuff.Name._char[startAddr++] = TagToType[DataTAG - 1][1]; // 数据类型 TYPE
    OutPutBuff.Name._char[startAddr++] = TagToType[DataTAG - 1][0]; // 数据标记 TAG
    OutPutBuff.Name._char[startAddr++] = VIF;   // VIF
    char DoubleBuff[8] = {0};
    switch (DataTAG) {
        case positiveHeat_TAG:
            break;
        case negativeHeat_TAG:
            break;
        case positiveFlow_TAG:
            break;
        case negativeFlow_TAG:
            break;
        case positiveSpeed_TAG:
            break;
        case negativeSpeed_TAG:
            break;
        case positiveTemperature_TAG:
            break;
        case negativeTemperature_TAG:
            break;
        case temperatureHigh_TAG:
            break;
        case positivePressure_TAG:
            break;
        case negativePressure_TAG:
            break;
        case pressureHigh_TAG:
            break;
        case devHigh_TAG:
            break;
        case powerRate_TAG:
            break;
        case voltage_TAG:
            OutPutBuff.Name._char[startAddr++] = (Now_DEV_Volt >> 8) & 0x00FF;
            OutPutBuff.Name._char[startAddr++] = (Now_DEV_Volt >> 0) & 0x00FF;
            break;
        case electricity_TAG:
            break;
        case workTime_TAG:
            break;
        case nowTime_TAG:
            OutPutBuff.Name._char[startAddr++] = 0x20;
            OutPutBuff.Name._char[startAddr++] = NowYear; //date
            OutPutBuff.Name._char[startAddr++] = NowMonth;
            OutPutBuff.Name._char[startAddr++] = NowDay;
            OutPutBuff.Name._char[startAddr++] = NowHour; //time
            OutPutBuff.Name._char[startAddr++] = NowMinute;
            OutPutBuff.Name._char[startAddr++] = NowSecond;
            break;
        case CCID_TAG:
            break;
        case IMEI_TAG:
            break;
        case CSQ_TAG:
            break;
        case RSRP_TAG:
            break;
        case RSRQ_TAG:
            break;
        case Moved_TAG:
            break;
        case SendClassObj_TAG:
            break;
        case RoomTemperArray_TAG:
            break;
        case upData_TAG:
            break;
        case SendCount_TAG:
            OutPutBuff.Name._char[startAddr++] = (Now_NetDevParameter.SendCount >> 8) & 0x00FF;
            OutPutBuff.Name._char[startAddr++] = (Now_NetDevParameter.SendCount >> 0) & 0x00FF;
            break;
        case receiveConut_TAG:
            OutPutBuff.Name._char[startAddr++] = (Now_NetDevParameter.ReceiveCount >> 8) & 0x00FF;
            OutPutBuff.Name._char[startAddr++] = (Now_NetDevParameter.ReceiveCount >> 0) & 0x00FF;
            break;
        case SOFT_VERSION_TAG:
            OutPutBuff.Name._char[startAddr++] = SOFT_VERSION;
            break;
        case NowTemper:
            DoubleOrFloatToBuff(NEW_NAME(DoubleBuff), Now_TemperOrPress, false);
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[0];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[1];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[2];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[3];
            break;
        case MessageResFlag:
            OutPutBuff.Name._char[startAddr++] = 0x01;  // 收到指令
            break;
        case ValveRate:
            break;
        case ValveFrequency:
            break;
        case FlagOfRights:
            break;
        case IPAndProt:
            break;
        case TemperSet_TAG:
            break;
        case Timeblock_TAG:
            break;
        case MeterAndTargeID_TAG:
            OutPutBuff.Name._char[startAddr++] = 0x00;
            OutPutBuff.Name._char[startAddr++] = 0x00;
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.MeterID[2];
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.MeterID[3];
            OutPutBuff.Name._char[startAddr++] = 0xFF;
            OutPutBuff.Name._char[startAddr++] = 0xFF;
            OutPutBuff.Name._char[startAddr++] = 0xFF;
            OutPutBuff.Name._char[startAddr++] = 0xFF;
            break;
        case NowRunMode_TAG:
            OutPutBuff.Name._char[startAddr++] = Now_NetDevParameter.NowRunTag;
            break;
        case NowPressure:
            DoubleOrFloatToBuff(NEW_NAME(DoubleBuff), Now_TemperOrPress, false);
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[0];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[1];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[2];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[3];
            break;
        case CurrentRSSI:
            break;
        case FrequencyPoint:
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.FrequencyPoint;
            break;
        case LoarSendInterval:
            OutPutBuff.Name._char[startAddr++] = (AT24CXX_Manager_NET.SendInterval >> 24) & 0x00FF;
            OutPutBuff.Name._char[startAddr++] = (AT24CXX_Manager_NET.SendInterval >> 16) & 0x00FF;
            OutPutBuff.Name._char[startAddr++] = (AT24CXX_Manager_NET.SendInterval >> 8) & 0x00FF;
            OutPutBuff.Name._char[startAddr++] = (AT24CXX_Manager_NET.SendInterval >> 0) & 0x00FF;
            break;
        case LoarSendFlag:
            OutPutBuff.Name._char[startAddr++] = 0x00;
            break;
        case MeterType_TAG:
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.Meter_Type;
            break;
        case ErrorCode:
            OutPutBuff.Name._char[startAddr++] = Now_NetDevParameter.ReceiveFlag;
            OutPutBuff.Name._char[startAddr++] = Current_meter_status;
            break;
        case PD1000_R:
            if (VIF == 0x01) {
                OutPutBuff.Name._char[startAddr++] = TEMP_R_VALUE_PT1 >> 8;
                OutPutBuff.Name._char[startAddr++] = TEMP_R_VALUE_PT1 & 0XFF;
            } else {
                OutPutBuff.Name._char[startAddr++] = TEMP_R_VALUE_PT2 >> 8;
                OutPutBuff.Name._char[startAddr++] = TEMP_R_VALUE_PT2 & 0XFF;
            }
            break;
    }
    OutPutBuff.Name._char[HeadStartAddr + 0] = ((startAddr - HeadStartAddr) >> 8) & 0x00FF;
    OutPutBuff.Name._char[HeadStartAddr + 1] = ((startAddr - HeadStartAddr) >> 0) & 0x00FF;
    return startAddr;
}
bool WeiteCmdSubData(strnew CmdSub) {
    /*  ResFlag:
        false: 不需要要写 eeprom 只改内存;
        0x02: 需要写 eeprom 但网络修改下不上传;
        _Module_EEpromSend 需要写 eeprom 且网络修改下会重发;
        _Module_EEpromTime 需要写 时间
    */
    uint8_t ResFlag;
    // int RXDataMaxLen = U8_Connect_U8(CmdSub.Name._char[0], CmdSub.Name._char[1]);
    if (CmdSub.Name._char[3] == nowTime_TAG) {
        if (TagToType[nowTime_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        FL_RTC_InitTypeDef TempTime;
        TempTime.year = CmdSub.Name._char[6];
        TempTime.month = CmdSub.Name._char[7];
        TempTime.day = CmdSub.Name._char[8];
        TempTime.hour = CmdSub.Name._char[9];
        TempTime.minute = CmdSub.Name._char[10];
        TempTime.second = CmdSub.Name._char[11];
        RTC_SetRTC(&TempTime);    // 设置时间结构体
        TempTime = RTC_Date;
        ResFlag = true;
    } else if ((CmdSub.Name._char[3] == NowTemper) || (CmdSub.Name._char[3] == NowPressure)) {
        if ((TagToType[NowTemper - 1][1] != CmdSub.Name._char[2]) || (TagToType[NowPressure - 1][1] != CmdSub.Name._char[2])) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        strnew FloatBuf = CmdSub;
        FloatBuf.Name._char = &CmdSub.Name._char[5];
        FloatBuf.MaxLen = 4;
        Now_TemperOrPress = (float)BuffToFloatOrDouble(FloatBuf, false);
        ResFlag = false;
    } else if ((CmdSub.Name._char[3] == SendCount_TAG) || (CmdSub.Name._char[3] == receiveConut_TAG)) {
        if ((TagToType[SendCount_TAG - 1][1] != CmdSub.Name._char[2]) && (TagToType[receiveConut_TAG - 1][1] != CmdSub.Name._char[2])) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        Now_NetDevParameter.ReceiveCount = 0;
        Now_NetDevParameter.SendCount = 0;
        ResFlag = false;
    } else if (CmdSub.Name._char[3] == MeterAndTargeID_TAG) {
        if (TagToType[MeterAndTargeID_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        AT24CXX_Manager_NET.MeterID[0] = 0x00;
        AT24CXX_Manager_NET.MeterID[1] = 0x00;
        AT24CXX_Manager_NET.MeterID[2] = (((unsigned char)CmdSub.Name._char[7] != 0xFF) ? CmdSub.Name._char[7] : AT24CXX_Manager_NET.MeterID[2]);
        AT24CXX_Manager_NET.MeterID[3] = (((unsigned char)CmdSub.Name._char[8] != 0xFF) ? CmdSub.Name._char[8] : AT24CXX_Manager_NET.MeterID[3]);
        ResFlag = true;
    } else if (CmdSub.Name._char[3] == NowRunMode_TAG) {
        if (TagToType[NowRunMode_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        Now_NetDevParameter.NowRunTag = CmdSub.Name._char[5];
        ResFlag = false;
    } else if (CmdSub.Name._char[3] == FrequencyPoint) {
        if (TagToType[FrequencyPoint - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        AT24CXX_Manager_NET.FrequencyPoint = CmdSub.Name._char[5];
        ResFlag = true;
    } else if (CmdSub.Name._char[3] == LoarSendInterval) {
        if (TagToType[LoarSendInterval - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        AT24CXX_Manager_NET.SendInterval = 0;
        for (int i = 0; i < 4; i++) {
            AT24CXX_Manager_NET.SendInterval = ((AT24CXX_Manager_NET.SendInterval << 8) & 0xFFFFFF00);
            AT24CXX_Manager_NET.SendInterval = CmdSub.Name._char[5 + i];
        }
        ResFlag = true;
    } else if (CmdSub.Name._char[3] == LoarSendFlag) {
        if (TagToType[LoarSendFlag - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        if (CmdSub.Name._char[5] == 0x01) {
            RTC_TASK.Task[SendLoarTask].CountMaxNum = 1;
        }
        ResFlag = false;
    } else if (CmdSub.Name._char[3] == MeterType_TAG) {
        if (TagToType[MeterType_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        AT24CXX_Manager_NET.Meter_Type = (CmdSub.Name._char[5] ? 1 : 2);
    } else {
        ResFlag = false;
    }
SubEndAndWriteData:
    return ResFlag;
}
bool ReadCmdSubData(strnew CmdSub) {
    return false;
}
bool CtrlCmdSubData(strnew CmdSub) {
#warning "need to be implemented";
    return false;
}
bool ResCmdSubData(strnew CmdSub) {
    return false;
}
bool DoingCmdSub(strnew CmdSub, int32_t ReadOrWriteFlag) {
    bool isResWriteEEprom;
    switch (ReadOrWriteFlag) {
        case CMD_CODE_WRITE:
            isResWriteEEprom = WeiteCmdSubData(CmdSub);
            break;
        case CMD_CODE_READ:
            break;
        case CMD_CODE_CTRL:
            isResWriteEEprom = CtrlCmdSubData(CmdSub);
            break;
        case CMD_CODE_RES:
            isResWriteEEprom = ResCmdSubData(CmdSub);
            break;
    }
    return isResWriteEEprom;
}

bool HY_USB_TTL_CheckBuff(char * RxBuf, int RxLen, uint8_t Now_LPUartx) {
    strnew RXDatabuff = {0};
    RXDatabuff.Name._char = RxBuf;
    RXDatabuff.MaxLen = RxLen;
    for (int i = 0; i < RxLen; i++) {
        if (RxBuf[i] == 0x68) {
            if (RxLen > i) {
                RXDatabuff.Name._char = RxBuf + i;
                RXDatabuff.MaxLen = RxLen - i;
            }
            break;
        }
        if (i == RxLen - 1) { // 无 0x68
            return false;
        }
    }
    int CsAddr = RXDatabuff.Name._char[10];
    if (CsAddr < 0 || CsAddr > RXDatabuff.MaxLen) {
        return false;
    }
    if ((RXDatabuff.Name._char[0] != 0x68) || ((RXDatabuff.Name._char[1] != 0x44))) {
        return false;
    }
    if (!(((uint8_t)RXDatabuff.Name._char[2] == 0xFF && (uint8_t)RXDatabuff.Name._char[3] == 0xFF && (uint8_t)RXDatabuff.Name._char[4] == 0xFF && (uint8_t)RXDatabuff.Name._char[5] == 0xFF)
        || (AT24CXX_Manager_NET.MeterID[2] == RXDatabuff.Name._char[4] && AT24CXX_Manager_NET.MeterID[3] == RXDatabuff.Name._char[5])
        )) {
        return false;
    }
    if (Cault_CS(RXDatabuff.Name._char, 0, CsAddr) != RXDatabuff.Name._char[CsAddr]) {
        return false;
    }
    bool isWirteEEprom = false;
    unsigned char EEprom_METERID[4] = {0};
    memcpy(EEprom_METERID, AT24CXX_Manager_NET.MeterID, 4);

    int SendDataLen = 0;
    if ((Now_LPUartx != 0xF1) && (Now_LPUartx != 0xF2)) {
        return false;
    }
    newstrobj(SendBuf, 1);
    SendBuf = (Now_LPUartx == 0xF1 ? NEW_NAME(LPUART0Ddata.TxBuf) : NEW_NAME(LPUART1Ddata.TxBuf));
    memset(SendBuf.Name._char, 0, SendBuf.MaxLen);
    SendBuf.Name._char[SendDataLen++] = 0x68;
    SendBuf.Name._char[SendDataLen++] = 0x44;
    SendBuf.Name._char[SendDataLen++] = 0xFF;
    SendBuf.Name._char[SendDataLen++] = 0xFF;
    SendBuf.Name._char[SendDataLen++] = 0xFF;
    SendBuf.Name._char[SendDataLen++] = 0xFF;
    SendBuf.Name._char[SendDataLen++] = 0x00;
    SendBuf.Name._char[SendDataLen++] = 0x11;
    SendBuf.Name._char[SendDataLen++] = 0x11;
    SendDataLen++;      // SendDataLen == 09 H 
    SendDataLen++;      // SendDataLen == 10 L 

    uint32_t CmdTempFlag = U8_Connect_U8(0x00, RXDatabuff.Name._char[11]);
    CmdTempFlag = (CmdTempFlag << 16 & 0xFFFF0000) | (U8_Connect_U8(RXDatabuff.Name._char[12], RXDatabuff.Name._char[13]) & 0x0000FFFF);
    if ((CmdTempFlag == CMD_CODE_CTRL || CmdTempFlag == CMD_CODE_WRITE)) {
        SendBuf.Name._char[SendDataLen++] = ((CMD_CODE_RES >> 0x10) & 0x00FF); // CMDID
        SendBuf.Name._char[SendDataLen++] = ((CMD_CODE_RES >> 0x08) & 0x00FF); // DI0
        SendBuf.Name._char[SendDataLen++] = ((CMD_CODE_RES >> 0x00) & 0x00FF); // DI1
    } else {
        SendBuf.Name._char[SendDataLen++] = ((CMD_CODE_WRITE >> 0x10) & 0x00FF); // CMDID
        SendBuf.Name._char[SendDataLen++] = ((CMD_CODE_WRITE >> 0x08) & 0x00FF); // DI0
        SendBuf.Name._char[SendDataLen++] = ((CMD_CODE_WRITE >> 0x00) & 0x00FF); // DI1
    }

    int SubLen = 0;
    char TempRXSubBuff[128] = {0};
    int RXDataMaxLen = U8_Connect_U8(RXDatabuff.Name._char[0x09], RXDatabuff.Name._char[0x0A]);
    int SteepAddr = 0;

    if (CmdTempFlag == CMD_CODE_RES) {
        DoingCmdSub(NEW_NAME(TempRXSubBuff), CmdTempFlag);
        return false;
    }
    for (int NowSubLenAddr = 14; NowSubLenAddr < RXDataMaxLen; NowSubLenAddr = 14 + SteepAddr) {
        memset(TempRXSubBuff, 0, ARR_SIZE(TempRXSubBuff));
        SubLen = U8_Connect_U8(RXDatabuff.Name._char[NowSubLenAddr], RXDatabuff.Name._char[NowSubLenAddr + 1]);
        if (SubLen > 128 || SubLen > RXDatabuff.MaxLen) {
            return false;
        }
        memcpy(TempRXSubBuff, &RXDatabuff.Name._char[NowSubLenAddr], (SubLen + 5));
        // 执行
        isWirteEEprom |= DoingCmdSub(NEW_NAME(TempRXSubBuff), CmdTempFlag);
        // 回复
        if ((CmdTempFlag == CMD_CODE_CTRL || CmdTempFlag == CMD_CODE_WRITE) && (NowSubLenAddr + (SubLen + 5) > RXDataMaxLen)) {
            SendDataLen = Add_HY_DataClass(NEW_NAME(SendBuf.Name._char), SendDataLen, MessageResFlag, 0x01);
        } else if (CmdTempFlag == CMD_CODE_READ) {
            SendDataLen = Add_HY_DataClass(NEW_NAME(SendBuf.Name._char), SendDataLen, RXDatabuff.Name._char[17 + SteepAddr], RXDatabuff.Name._char[18 + SteepAddr]);
        } else {
            SendBuf.Name._char[SendDataLen++] = 0x00;
            SendBuf.Name._char[SendDataLen++] = 0x06;
            SendBuf.Name._char[SendDataLen++] = TagToType[MessageResFlag - 1][1]; // 数据类型 TYPE
            SendBuf.Name._char[SendDataLen++] = TagToType[MessageResFlag - 1][0]; // 数据标记 TAG
            SendBuf.Name._char[SendDataLen++] = MessageResFlag;                   // VIF
            SendBuf.Name._char[SendDataLen++] = 0x00;                             // 回复指令错误
        }
        SteepAddr += U8_Connect_U8(RXDatabuff.Name._char[14 + SteepAddr], RXDatabuff.Name._char[15 + SteepAddr]);
    }
    SendBuf.Name._char[0x09] = ((SendDataLen >> 8) & 0x00FF);
    SendBuf.Name._char[0x0A] = ((SendDataLen >> 0) & 0x00FF);
    SendBuf.Name._char[SendDataLen] = get_CheckSum((unsigned char *)SendBuf.Name._char, SendDataLen);
    SendBuf.Name._char[++SendDataLen] = 0x16;
    if (Now_LPUartx == 0xF1) {
        LPUART0Ddata.TxLen = ++SendDataLen;
    } else {
        LPUART1Ddata.TxLen = ++SendDataLen;
    }
    if (isWirteEEprom == true) {
        EEprom_AT24CXX_Parameter_Init(true);
    }
    return true;
}
#include "SX1276.h"
void Device_Init(void) {
    LPUART_Init(LPUART0);   // 初始化 LPUart0
    LPUART_Init(LPUART1);   // 初始化 LPUart1
    // RTC 开启定时器中断
    MF_RTC_1S_Init();
    // LPTIM 开启定时器中断
    // LPTIM_Setup();
    // LPTIM_Stop();
    // BSTIM 开启定时器中断
    // BSTIM32_Setup();    //开启定时器中断
    // BSTIM32_Stop();     //关闭定时器中断
    RTC_TASK.InitSetTimeTask(IWDTClS, MinToSec(8), NULL); // 8min 内定时器喂狗
}
void StartOPenDevMode(void) {
    Device_Init();
    /* Initial IWDT */
    MF_IWDT_Init();
    FL_RTC_InitTypeDef TempTime;
    ReadTime_EEprom(&TempTime);
    RTC_SetRTC(&TempTime);    // 设置时间结构体
    IncludeDelayMs(1000);
    Config_Init_XTHF_And_XTLF();
    MF_LPUART0_Interrupt_Init();    // 开启 LPUart0
    MF_LPUART1_Interrupt_Init();    // 开启 LPUart1
    SX1276LoRa_GPIO_Init();
}
void CheckMeterNum(void) {
    if (AT24CXX_Manager_NET.MeterTemperature_Adjust == 0xFFFF) {
        AT24CXX_Manager_NET.MeterTemperature_Adjust = 0;
        EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.MeterTemperature_Adjust, sizeof(AT24CXX_Manager_NET.MeterTemperature_Adjust));
    }
    if (AT24CXX_Manager_NET.MeterPress_Adjust == 0xFFFF) {
        AT24CXX_Manager_NET.MeterPress_Adjust = 0;
        EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.MeterPress_Adjust, sizeof(AT24CXX_Manager_NET.MeterPress_Adjust));
    }
}
#include "StrLib.h"
#define IS_NVIC_VECTTAB(VECTTAB)  (((VECTTAB) == 0x00000000) || ((VECTTAB) == 0x08000000) || ((VECTTAB) == 0x20000000))
#define IS_NVIC_OFFSET(OFFSET)  (((OFFSET) & 0x1FFFFF80) == (OFFSET))
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset) {
    /*Check the parameters */
    assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
    assert_param(IS_NVIC_OFFSET(Offset));
    SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}

