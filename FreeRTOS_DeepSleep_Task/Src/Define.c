#include "Define.h"
#include "LPUart_0_And_1_Lib.h"
#include "AT24CXXDataLoader.h"
#include "UpData.h"

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
    {ValveSturt,Type_ValveData_obj},
    {SetValve_TAG,Type_uint8_t},
    {LimitTime_TAG,Type_ASCII},
    {HotInsidTRang_TAG,Type_HEXS},
    {T2CtrlData_TAG,Typ_T2Ctrl_obj},
    {OpenValveRunTime_TAG,Type_uint32_t},
};

const char SOFT_VERSION = 11; // 版本号
unsigned int UART_BAND = 115200;

// LongUART UART1Ddata;
ShortUART LPUART0Ddata, LPUART1Ddata;

// 校验计算
unsigned char Cault_CS(char * Buffer, unsigned char start_num, unsigned char stop_num) {
    unsigned char temp, cs;
    cs = 0;
    for (temp = start_num; temp < stop_num; temp++) {
        cs = cs + *(Buffer + temp);
    }
    return cs;
}
int floatToUint6410Rate(float Num) {
    int TempNum = (int)((Num + 0.05) * 10);
    TempNum = (TempNum > 0 ? TempNum : 0 - TempNum);
    return TempNum;
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
            DoubleOrFloatToBuff(NEW_NAME(DoubleBuff), Get_Module_Data.Now_Temper_T1, false);
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[0];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[1];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[2];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[3];
            break;
        case negativeTemperature_TAG:
            DoubleOrFloatToBuff(NEW_NAME(DoubleBuff), Get_Module_Data.Now_Temper_T2, false);
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[0];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[1];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[2];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[3];
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
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.Now_DEV_Volt >> 8) & 0x00FF;
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.Now_DEV_Volt >> 0) & 0x00FF;
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
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.CtrlDev_SendCount >> 8) & 0x00FF;
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.CtrlDev_SendCount >> 0) & 0x00FF;
            break;
        case receiveConut_TAG:
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.CtrlDev_ReceiveCount >> 8) & 0x00FF;
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.CtrlDev_ReceiveCount >> 0) & 0x00FF;
            break;
        case SOFT_VERSION_TAG:
            OutPutBuff.Name._char[startAddr++] = SOFT_VERSION;
            break;
        case NowTemper:
            break;
        case MessageResFlag:
            OutPutBuff.Name._char[startAddr++] = 0x01;  // 收到指令
            break;
        case ValveRate:
            break;
        case ValveFrequency:
            break;
        case FlagOfRights:
            OutPutBuff.Name._char[startAddr++] = ((AT24CXX_Manager_NET.ModeCode >> 8) & 0x00FF);
            OutPutBuff.Name._char[startAddr++] = ((AT24CXX_Manager_NET.ModeCode >> 0) & 0x00FF);
            break;
        case IPAndProt:
            break;
        case TemperSet_TAG:
            OutPutBuff.Name._char[startAddr++] = Get_Module_Data.CtrlDev_TemperSet;
            break;
        case Timeblock_TAG:
            break;
        case MeterAndTargeID_TAG:
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.MeterID[0];
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.MeterID[1];
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.MeterID[2];
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.MeterID[3];
            OutPutBuff.Name._char[startAddr++] = 0xFF;
            OutPutBuff.Name._char[startAddr++] = 0xFF;
            OutPutBuff.Name._char[startAddr++] = 0xFF;
            OutPutBuff.Name._char[startAddr++] = 0xFF;
            break;
        case NowRunMode_TAG:
            OutPutBuff.Name._char[startAddr++] = Get_Module_Data.CtrlDev_NowRunMode;
            break;
        case NowPressure:
            break;
        case CurrentRSSI:
            break;
        case FrequencyPoint:
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.MeterID[3];
            break;
        case LoarSendInterval:
            break;
        case LoarSendFlag:
            OutPutBuff.Name._char[startAddr++] = 0x00;
            break;
        case MeterType_TAG:
            break;
        case ErrorCode:
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.StytemErrorCode >> 8 & 0x00FF);
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.StytemErrorCode >> 0 & 0x00FF);
            break;
        case PD1000_R:
            break;
        case ValveSturt: {
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.CtrlDev_Now_Volt >> 8) & 0x00FF;  // 温控电池电压
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.CtrlDev_Now_Volt >> 0) & 0x00FF;  // 温控电池电压
            OutPutBuff.Name._char[startAddr++] = Get_Module_Data.CtrlDev_NowRunMode;        // 模式
            OutPutBuff.Name._char[startAddr++] = Get_Module_Data.CtrlDev_TemperRoom;        // 室内温度
            OutPutBuff.Name._char[startAddr++] = Get_Module_Data.CtrlDev_TemperSet;         // 设定温度
            OutPutBuff.Name._char[startAddr++] = Get_Module_Data.CtrlDev_SendCount;         // 发送次数
            OutPutBuff.Name._char[startAddr++] = Get_Module_Data.CtrlDev_ReceiveCount;      // 接收次数
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.Now_Degree_Part >> 8) & 0x00FF;   // 当前角度
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.Now_Degree_Part >> 0) & 0x00FF;   // 当前角度
            // 截止日期
            memcpy(&OutPutBuff.Name._char[startAddr], AT24CXX_Manager_NET.LimitTime, strlen(AT24CXX_Manager_NET.LimitTime));
            startAddr += strlen(AT24CXX_Manager_NET.LimitTime);
            // 进水温度
            int TemperBuf = floatToUint6410Rate(Get_Module_Data.Now_Temper_T1);
            OutPutBuff.Name._char[startAddr++] = TemperBuf % 256;
            OutPutBuff.Name._char[startAddr++] = TemperBuf / 256;
            // 回水温度
            TemperBuf = floatToUint6410Rate(Get_Module_Data.Now_Temper_T2);
            OutPutBuff.Name._char[startAddr++] = TemperBuf % 256;
            OutPutBuff.Name._char[startAddr++] = TemperBuf / 256;
            // 累计开阀时间
            uint32_t RunTimeNum = 0;
            EEprom_Read_Start((unsigned char *)&RunTimeNum, EEPROM_OTHER_ADDRESS + (AT24CXX_Manager_NET.RunTimeBack_AddrNum * 4), 4);
            OutPutBuff.Name._char[startAddr++] = ((RunTimeNum >> 24) & 0x00FF);
            OutPutBuff.Name._char[startAddr++] = ((RunTimeNum >> 16) & 0x00FF);
            OutPutBuff.Name._char[startAddr++] = ((RunTimeNum >> 8) & 0x00FF);
            OutPutBuff.Name._char[startAddr++] = ((RunTimeNum >> 0) & 0x00FF);
            // 权限
            OutPutBuff.Name._char[startAddr++] = ((AT24CXX_Manager_NET.ModeCode >> 8) & 0x00FF);
            OutPutBuff.Name._char[startAddr++] = ((AT24CXX_Manager_NET.ModeCode >> 0) & 0x00FF);
            OutPutBuff.Name._char[startAddr++] = Get_Module_Data.FronValveDoneCode;  // 动作由来
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.Now_DEV_Volt >> 8) & 0x00FF;  // 电池电压
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.Now_DEV_Volt >> 0) & 0x00FF;  // 电池电压
            OutPutBuff.Name._char[startAddr++] = (Get_Module_Data.Now_Degree_Part < 100 ? 0x99 : 0x55);  // 阀门状态
            OutPutBuff.Name._char[startAddr++] = SOFT_VERSION;  // 程序版本
            // RTC时钟
            OutPutBuff.Name._char[startAddr++] = 0x20;
            OutPutBuff.Name._char[startAddr++] = NowYear; //date
            OutPutBuff.Name._char[startAddr++] = NowMonth;
            OutPutBuff.Name._char[startAddr++] = NowDay;
            OutPutBuff.Name._char[startAddr++] = NowHour; //time
            OutPutBuff.Name._char[startAddr++] = NowMinute;
            OutPutBuff.Name._char[startAddr++] = NowSecond;
            break;
        }
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
    } else if (CmdSub.Name._char[3] == positiveTemperature_TAG) {
        if ((TagToType[NowTemper - 1][1] != CmdSub.Name._char[2]) || (TagToType[NowPressure - 1][1] != CmdSub.Name._char[2])) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        strnew FloatBuf = CmdSub;
        FloatBuf.Name._char = &CmdSub.Name._char[5];
        FloatBuf.MaxLen = 4;
        Get_Module_Data.Now_Temper_T1 = (float)BuffToFloatOrDouble(FloatBuf, false);
        ResFlag = false;
    }if (CmdSub.Name._char[3] == negativeTemperature_TAG) {
        if ((TagToType[NowTemper - 1][1] != CmdSub.Name._char[2]) || (TagToType[NowPressure - 1][1] != CmdSub.Name._char[2])) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        strnew FloatBuf = CmdSub;
        FloatBuf.Name._char = &CmdSub.Name._char[5];
        FloatBuf.MaxLen = 4;
        Get_Module_Data.Now_Temper_T2 = (float)BuffToFloatOrDouble(FloatBuf, false);
        ResFlag = false;
    } else if ((CmdSub.Name._char[3] == SendCount_TAG) || (CmdSub.Name._char[3] == receiveConut_TAG)) {
        if ((TagToType[SendCount_TAG - 1][1] != CmdSub.Name._char[2]) && (TagToType[receiveConut_TAG - 1][1] != CmdSub.Name._char[2])) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        Get_Module_Data.CtrlDev_ReceiveCount = 0;
        Get_Module_Data.CtrlDev_SendCount = 0;
        ResFlag = false;
    } else if (CmdSub.Name._char[3] == MeterAndTargeID_TAG) {
        if (TagToType[MeterAndTargeID_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        AT24CXX_Manager_NET.MeterID[0] = (((unsigned char)CmdSub.Name._char[5] != 0xFF) ? CmdSub.Name._char[5] : AT24CXX_Manager_NET.MeterID[0]);
        AT24CXX_Manager_NET.MeterID[1] = (((unsigned char)CmdSub.Name._char[6] != 0xFF) ? CmdSub.Name._char[6] : AT24CXX_Manager_NET.MeterID[1]);
        AT24CXX_Manager_NET.MeterID[2] = (((unsigned char)CmdSub.Name._char[7] != 0xFF) ? CmdSub.Name._char[7] : AT24CXX_Manager_NET.MeterID[2]);
        AT24CXX_Manager_NET.MeterID[3] = (((unsigned char)CmdSub.Name._char[8] != 0xFF) ? CmdSub.Name._char[8] : AT24CXX_Manager_NET.MeterID[3]);
        ResFlag = true;
    } else if (CmdSub.Name._char[3] == SetValve_TAG) { // 设定角度
        if (TagToType[MeterAndTargeID_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        Get_Module_Data.Now_Degree_Part = U8_Connect_U8(CmdSub.Name._char[5], CmdSub.Name._char[6]);

        ResFlag = true;
    } else if (CmdSub.Name._char[3] == LimitTime_TAG) { // 截止日期
        if (TagToType[MeterAndTargeID_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        memcpy(&AT24CXX_Manager_NET.LimitTime, &CmdSub.Name._char[5], strlen("1999-05-20 13:14:00"));
        ResFlag = true;
    } else if (CmdSub.Name._char[3] == FlagOfRights) { // 设置权限
        if (TagToType[MeterAndTargeID_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        AT24CXX_Manager_NET.ModeCode = U8_Connect_U8(CmdSub.Name._char[5], CmdSub.Name._char[6]);
        ResFlag = true;
    } else if (CmdSub.Name._char[3] == HotInsidTRang_TAG) { // 修改热力公司要求的温度范围
        if (TagToType[MeterAndTargeID_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        AT24CXX_Manager_NET.InsetT_Max = CmdSub.Name._char[5];
        AT24CXX_Manager_NET.InsetT_Min = CmdSub.Name._char[6];
        ResFlag = true;
    } else if (CmdSub.Name._char[3] == T2CtrlData_TAG) { // 设置 T2 自动调节参数
        if (TagToType[MeterAndTargeID_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        for (uint8_t i = 0; i < 4; i++) {
            AT24CXX_Manager_NET.T2AutoCtrlBuf[i].T2Max = CmdSub.Name._char[5 + i * 4];
            AT24CXX_Manager_NET.T2AutoCtrlBuf[i].T2Min = CmdSub.Name._char[6 + i * 4];
            AT24CXX_Manager_NET.T2AutoCtrlBuf[i].T2_Set_Degree_Part = U8_Connect_U8(CmdSub.Name._char[7 + i * 4], CmdSub.Name._char[8 + i * 4]);
        }
        ResFlag = true;
    } else if (CmdSub.Name._char[3] == OpenValveRunTime_TAG) { // 设置累计开阀时间
        if (TagToType[MeterAndTargeID_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        // 累计开阀时间
        uint32_t RunTimeNum = 0;
        RunTimeNum = U8_Connect_U8(CmdSub.Name._char[5], CmdSub.Name._char[6]);
        RunTimeNum = RunTimeNum << 16;
        RunTimeNum = (RunTimeNum & 0xFFFF0000) | (U8_Connect_U8(CmdSub.Name._char[7], CmdSub.Name._char[8]) & 0x0000FFFF);
        EEprom_Write_Start((unsigned char *)&RunTimeNum, EEPROM_OTHER_ADDRESS + (AT24CXX_Manager_NET.RunTimeBack_AddrNum * 4), 4);
        ResFlag = true;
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
    if ((RXDatabuff.Name._char[0] != 0x68) || ((RXDatabuff.Name._char[1] != 0x53))) {
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
    SendBuf.Name._char[SendDataLen++] = 0x53;
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
    if (CmdTempFlag == CMD_CODE_UPDATA) {
        newstrobj(JsonInputStr, 1);
        JsonInputStr.Name._char = &RXDatabuff.Name._char[19];
        JsonInputStr.MaxLen = U8_Connect_U8(RXDatabuff.Name._char[14], RXDatabuff.Name._char[15]);
        if (RXDatabuff.MaxLen < JsonInputStr.MaxLen + 14) {
            return false;   // 计算长度大于最大值, 直接退出
        }
        JsonInputStr.Name._char[JsonInputStr.MaxLen] = 0; // 把CS改为 \0 截断字符串
        int temp = UpData_Receive_Hex(newJsonObjectByString(JsonInputStr));
        if (temp == 0) {
            SendBuf.Name._char[14] = 0x00;
            SendBuf.Name._char[15] = strlen("{\"upgrades\":\"start\"}") + 5;
            SendDataLen += catString(&SendBuf.Name._char[SendDataLen], "{\"upgrades\":\"start\"}", (512 - SendDataLen), strlen("{\"upgrades\":\"start\"}"));
        } else if (temp == 1) {
            newString(ResStr, 60);
            sprintf(ResStr.Name._char, "{\"NowPackNum\":%d,\"upgrades\":\"fail\"}", UpdataData.NowPackNum);
            SendBuf.Name._char[14] = 0x00;
            SendBuf.Name._char[15] = ResStr.getStrlen(&ResStr) + 5;
            SendDataLen += catString(&SendBuf.Name._char[SendDataLen], ResStr.Name._char, (512 - SendDataLen), ResStr.MaxLen);
        } else if (temp == 2) {
            newString(ResStr, 60);
            sprintf(ResStr.Name._char, "{\"NowPackNum\":%d,\"upgrades\":\"OK\"}", UpdataData.NowPackNum);
            SendBuf.Name._char[14] = 0x00;
            SendBuf.Name._char[15] = ResStr.getStrlen(&ResStr) + 5;
            SendDataLen += catString(&SendBuf.Name._char[SendDataLen], ResStr.Name._char, (512 - SendDataLen), ResStr.MaxLen);
        } else if (temp == 3) {
            SendBuf.Name._char[14] = 0x00;
            SendBuf.Name._char[15] = strlen("{\"upgrades\":\"over\"}") + 5;
            SendDataLen += catString(&SendBuf.Name._char[SendDataLen], "{\"upgrades\":\"over\"}", (512 - SendDataLen), strlen("{\"upgrades\":\"over\"}"));
        }
        SendBuf.Name._char[0x09] = ((SendDataLen >> 8) & 0x00FF);
        SendBuf.Name._char[0x0A] = ((SendDataLen >> 0) & 0x00FF);
    } else {
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
    }

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

#include "TaskGpioInit.h"
#include "RF_CMT2300A_Init.h"
#include "../Interflow/Check_XTHF_Or_XTLF.h"
void InfraredCtrlSub(void) {
    LPUART0_Init(true);
    Infrared_POWER_OFF;
}
void Device_Init(void) {
    InfraredCtrlSub();
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
    MF_EXTI_Line_Init(); // 配置外部中断对应的端口
    FL_RTC_InitTypeDef TempTime;
    ReadTime_EEprom(&TempTime);
    RTC_SetRTC(&TempTime);    // 设置时间结构体
    IncludeDelayMs(1000);
    Config_Init_XTHF_And_XTLF();
    MF_LPUART0_Interrupt_Init();    // 开启 LPUart0
    // MF_LPUART1_Interrupt_Init();    // 开启 LPUart1
    CMT2300A_GPIO_Init();
}
void CheckMeterNum(void) {
    // if (AT24CXX_Manager_NET.MeterTemperature_Adjust == 0xFFFF) {
    //     AT24CXX_Manager_NET.MeterTemperature_Adjust = 0;
    //     EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.MeterTemperature_Adjust, sizeof(AT24CXX_Manager_NET.MeterTemperature_Adjust));
    // }
    // if (AT24CXX_Manager_NET.MeterPress_Adjust == 0xFFFF) {
    //     AT24CXX_Manager_NET.MeterPress_Adjust = 0;
    //     EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.MeterPress_Adjust, sizeof(AT24CXX_Manager_NET.MeterPress_Adjust));
    // }
}

void SetErrerCode(CodeErr ErrId, bool UserSet) {
    Get_Module_Data.StytemErrorCode = setDataBit(Get_Module_Data.StytemErrorCode, ErrId, UserSet);
}

NetDevParameter Get_Module_Data = {
    .ReceiveFlag = false,
    .isOpen433 = false,
    .StytemErrorCode = 0,
    .CtrlDev_Now_Volt = 3600,
    .CtrlDev_NowRunMode = 0,
    .CtrlDev_TemperRoom = 0,
    .CtrlDev_TemperSet = 0,
    .CtrlDev_Set_Degree_Part = 0,
    .CtrlDev_SendCount = 0,
    .CtrlDev_ReceiveCount = 0,
    .RunTimeFor433 = 0,
    .TempRunTimeForValve = 0,
    .CtrlDev_CMT2300A_RF_TX_dbm = 0,
    .CtrlDev_CMT2300A_RF_RX_dbm = 0,
    .Now_DEV_Volt = 3600,
    .Now_Temper_T1 = 0.0,
    .Now_Temper_T2 = 0.0,
    .Now_Degree_Part = 0,
};

void CMT_SPI_GPIO1_Set_In(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_10;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void CMT_SPI_SDIO_Set_Out(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_3;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void CMT_SPI_SDIO_Set_In(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_3;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
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

