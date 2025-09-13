#include "Define.h"
#include "LPUart_0_And_1_Lib.h"
#include "AT24CXXDataLoader.h"
#include "UpData.h"
#include "MotorCtrlDev.h"
#include "Task_Valve.h"
#include "Display.h"
#include "TaskAboutTimer.h"
#include "TaskNBAndWeb.h"
#include "NetProt_Module.h"

unsigned char _Module_Start_Flage = 0;
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
    {RebootDev_TAG,Type_bin},
    {TesTLCD_TAG,Type_bin},
};

const char SOFT_VERSION = 11; // 版本号
unsigned int UART_BAND = 115200;

LongUART LPUART0Ddata, UART1Ddata;
// ShortUART LPUART1Ddata;

// void _EnterQueue(struct _MeterData This, double Temperature);
// double _DeleteQueue(struct _MeterData This);
// MeterData Meter_Manager = {
//     .DataCount = 0,
//     .DataStartHour = 0,
//     .MeterTemperature = {0},
//     .EnterQueue = _EnterQueue,
//     .DeleteQueue = _DeleteQueue,
// };
// // 入队
// void _EnterQueue(struct _MeterData This, double Temperature) {
//     if (Meter_Manager.DataCount == 24) {   // 队列满
//         This.DeleteQueue(&This);    // 出队一个元素
//     }
//     char TempAddr = (Meter_Manager.DataCount + Meter_Manager.DataStartHour) % 24;
//     getStrUserTime(RTC_Date, Meter_Manager.MeterTemperature[TempAddr].CollectionTime);
//     Meter_Manager.MeterTemperature[TempAddr].RoomTemperature = Temperature;
//     Meter_Manager.DataCount = ((Meter_Manager.DataCount < 24) ? Meter_Manager.DataCount + 1 : 24);
//     return;
// }
// // 出队
// double _DeleteQueue(struct _MeterData This) {
//     if (Meter_Manager.DataCount == 0) {   // 队列空
//         return 0;
//     }
//     double Temperature = Meter_Manager.MeterTemperature[Meter_Manager.DataStartHour].RoomTemperature;
//     Meter_Manager.DataStartHour = (Meter_Manager.DataStartHour + 1) % 24;// 队首指针后移
//     Meter_Manager.DataCount--; // 队列长度减一
//     return Temperature;
// }

// 校验计算
unsigned char Cault_CS(void * Buffer, unsigned char start_num, unsigned int stop_num) {
    unsigned int temp = 0;
    unsigned char cs = 0;
    uint8_t * Buff_Ptr = Buffer;
    for (temp = start_num; temp < stop_num; temp++) {
        cs = cs + Buff_Ptr[temp];
    }
    return cs;
}
int floatToint6410Rate(float Num) {
    int TempNum = (int)((Num + 0.05) * 10);
    TempNum = (TempNum > 0 ? TempNum : 0 - TempNum);
    return TempNum;
}
int Add_HY_DataClass(strnew OutPutBuff, int startAddr, unsigned char DataTAG, unsigned char VIF) {
    unsigned char EEprom_IMEI[15] = {0};
    unsigned char EEprom_CCID[11] = {0};
    memcpy(EEprom_IMEI, AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_IMEI, 15);
    strnew CCIDStr = NEW_NAME(AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_ICCID);
    int Len = CCIDStr.getStrlen(&CCIDStr);
    AsciiToHex(CCIDStr.Name._char, Len, (char *)EEprom_CCID, 10);

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
            DoubleOrFloatToBuff(NEW_NAME(DoubleBuff), System_RunData.Now_Temper_T1, false);
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[0];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[1];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[2];
            OutPutBuff.Name._char[startAddr++] = DoubleBuff[3];
            break;
        case negativeTemperature_TAG:
            DoubleOrFloatToBuff(NEW_NAME(DoubleBuff), System_RunData.Now_Temper_T2, false);
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
            OutPutBuff.Name._char[startAddr++] = (System_RunData.Now_DEV_Volt >> 8) & 0x00FF;
            OutPutBuff.Name._char[startAddr++] = (System_RunData.Now_DEV_Volt >> 0) & 0x00FF;
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
            for (int i = 0; i < 10; i++) {
                OutPutBuff.Name._char[startAddr++] = EEprom_CCID[i];
            }
            break;
        case IMEI_TAG:
            OutPutBuff.Name._char[startAddr++] = (EEprom_IMEI[0] & 0x0F);
            OutPutBuff.Name._char[startAddr++] = ((EEprom_IMEI[1] & 0x0F) << 4) + (EEprom_IMEI[2] & 0x0F);
            OutPutBuff.Name._char[startAddr++] = ((EEprom_IMEI[3] & 0x0F) << 4) + (EEprom_IMEI[4] & 0x0F);
            OutPutBuff.Name._char[startAddr++] = ((EEprom_IMEI[5] & 0x0F) << 4) + (EEprom_IMEI[6] & 0x0F);
            OutPutBuff.Name._char[startAddr++] = ((EEprom_IMEI[7] & 0x0F) << 4) + (EEprom_IMEI[8] & 0x0F);
            OutPutBuff.Name._char[startAddr++] = ((EEprom_IMEI[9] & 0x0F) << 4) + (EEprom_IMEI[10] & 0x0F);
            OutPutBuff.Name._char[startAddr++] = ((EEprom_IMEI[11] & 0x0F) << 4) + (EEprom_IMEI[12] & 0x0F);
            OutPutBuff.Name._char[startAddr++] = ((EEprom_IMEI[13] & 0x0F) << 4) + (EEprom_IMEI[14] & 0x0F);
            break;
        case CSQ_TAG:
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_CSQ;
            break;
        case RSRP_TAG:
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_RSRP % 0xFF;
            break;
        case RSRQ_TAG:
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_RSRQ % 0xFF;
            break;
        case Moved_TAG:
            break;
        case SendClassObj_TAG:
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.SendManageObj.SendIntervalDay >> 24;
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.SendManageObj.SendIntervalDay >> 16;
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.SendManageObj.SendIntervalDay >> 8;
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.SendManageObj.SendIntervalDay;
            OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.SendManageObj.SendStartHour;
            for (int i = 0; i < 4; i++) {
                OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.SendManageObj.SendFlagMode[i].StartDay >> 8;
                OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.SendManageObj.SendFlagMode[i].StartDay;
                OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.SendManageObj.SendFlagMode[i].SendInterval;
                OutPutBuff.Name._char[startAddr++] = AT24CXX_Manager_NET.SendManageObj.SendFlagMode[i].OneDayMaxSendTime;
            }
            break;
        case RoomTemperArray_TAG:
            break;
        case upData_TAG:
            break;
        case SendCount_TAG:
            break;
        case receiveConut_TAG:
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
            sprintf(&OutPutBuff.Name._char[startAddr], "%s:%d", \
                AT24CXX_Manager_NET.NET_Remote_Url, AT24CXX_Manager_NET.NET_Remote_Port);
            startAddr += strlen(&OutPutBuff.Name._char[startAddr]);
            break;
        case TemperSet_TAG:
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
            break;
        case PD1000_R:
            break;
        case ValveSturt: {
            uint16_t InputData = 0;
            if (System_RunData.Now_Degree_Part > DegreePartToUint(95)) {
                InputData = DegreePartToUint(100);
            } else if (System_RunData.Now_Degree_Part < DegreePartToUint(4)) {
                InputData = 0;
            } else {
                InputData = System_RunData.Now_Degree_Part;
            }
            OutPutBuff.Name._char[startAddr++] = 0x00;  // (System_RunData.CtrlDev_Now_Volt >> 8) & 0x00FF;  // 温控电池电压
            OutPutBuff.Name._char[startAddr++] = 0x00;  // (System_RunData.CtrlDev_Now_Volt >> 0) & 0x00FF;  // 温控电池电压
            OutPutBuff.Name._char[startAddr++] = 0x00;  // System_RunData.CtrlDev_NowRunMode;        // 模式
            OutPutBuff.Name._char[startAddr++] = 0x00;  // System_RunData.CtrlDev_TemperRoom;        // 室内温度
            OutPutBuff.Name._char[startAddr++] = 0x00;  // System_RunData.CtrlDev_TemperSet;         // 设定温度
            OutPutBuff.Name._char[startAddr++] = 0x00;  // System_RunData.CtrlDev_SendCount;         // 发送次数
            OutPutBuff.Name._char[startAddr++] = 0x00;  // System_RunData.CtrlDev_ReceiveCount;      // 接收次数
            OutPutBuff.Name._char[startAddr++] = (InputData >> 8) & 0x00FF;   // 当前角度
            OutPutBuff.Name._char[startAddr++] = (InputData >> 0) & 0x00FF;   // 当前角度
            // 截止日期
            memcpy(&OutPutBuff.Name._char[startAddr], AT24CXX_Manager_NET.LimitTime, strlen(AT24CXX_Manager_NET.LimitTime));
            startAddr += strlen(AT24CXX_Manager_NET.LimitTime);
            // 进水温度
            int TemperBuf = floatToint6410Rate(System_RunData.Now_Temper_T1);
            OutPutBuff.Name._char[startAddr++] = TemperBuf % 256;
            OutPutBuff.Name._char[startAddr++] = TemperBuf / 256;
            // 回水温度
            TemperBuf = floatToint6410Rate(System_RunData.Now_Temper_T2);
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
            OutPutBuff.Name._char[startAddr++] = System_RunData.FronValveDoneCode; // 动作由来
            OutPutBuff.Name._char[startAddr++] = (System_RunData.Now_DEV_Volt >> 8) & 0x00FF;  // 电池电压
            OutPutBuff.Name._char[startAddr++] = (System_RunData.Now_DEV_Volt >> 0) & 0x00FF;  // 电池电压
            OutPutBuff.Name._char[startAddr++] = (InputData < DegreePartToUint(100) ? 0x99 : 0x55);  // 阀门状态
            OutPutBuff.Name._char[startAddr++] = SOFT_VERSION;  // 程序版本
            // RTC时钟
            OutPutBuff.Name._char[startAddr++] = 0x20;
            OutPutBuff.Name._char[startAddr++] = NowYear; //date
            OutPutBuff.Name._char[startAddr++] = NowMonth;
            OutPutBuff.Name._char[startAddr++] = NowDay;
            OutPutBuff.Name._char[startAddr++] = NowHour; //time
            OutPutBuff.Name._char[startAddr++] = NowMinute;
            OutPutBuff.Name._char[startAddr++] = NowSecond;
            // 阀门故障码
            OutPutBuff.Name._char[startAddr++] = (System_RunData.StytemErrorCode >> 8 & 0x00FF);
            OutPutBuff.Name._char[startAddr++] = (System_RunData.StytemErrorCode >> 0 & 0x00FF);
            break;
        }
    }
    OutPutBuff.Name._char[HeadStartAddr + 0] = ((startAddr - HeadStartAddr) >> 8) & 0x00FF;
    OutPutBuff.Name._char[HeadStartAddr + 1] = ((startAddr - HeadStartAddr) >> 0) & 0x00FF;
    return startAddr;
}
#include "TaskAboutTimer.h"
bool WeiteCmdSubData(strnew CmdSub) {
    /*  ResFlag:
        false: 不需要要写 eeprom 只改内存;
        0x02: 需要写 eeprom 但网络修改下不上传;
        _Module_EEpromSend 需要写 eeprom 且网络修改下会重发;
        _Module_EEpromTime 需要写 时间;
    */
    uint8_t ResFlag;
    int RXDataMaxLen = U8_Connect_U8(CmdSub.Name._char[0], CmdSub.Name._char[1]);
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
        setIsWriteEEprom(_Module_EEpromTime);   // 准备写时间
        ResFlag = true;
        LCD_MenuPage_Change('5'); // 跳转显示
        // 开启截至日期判断任务
        StartLimitCheckTask();
    } else if (CmdSub.Name._char[3] == positiveTemperature_TAG) {
        if (TagToType[positiveTemperature_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        strnew FloatBuf = CmdSub;
        FloatBuf.Name._char = &CmdSub.Name._char[5];
        FloatBuf.MaxLen = 4;
        System_RunData.Now_Temper_T1 = (float)BuffToFloatOrDouble(FloatBuf, false);
        ResFlag = false;
        LCD_MenuPage_Change('3'); // 跳转显示
    } else if (CmdSub.Name._char[3] == negativeTemperature_TAG) {
        if (TagToType[negativeTemperature_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        strnew FloatBuf = CmdSub;
        FloatBuf.Name._char = &CmdSub.Name._char[5];
        FloatBuf.MaxLen = 4;
        System_RunData.Now_Temper_T2 = (float)BuffToFloatOrDouble(FloatBuf, false);
        ResFlag = false;
        LCD_MenuPage_Change('3'); // 跳转显示
    } else if (CmdSub.Name._char[3] == MeterAndTargeID_TAG) {
        if (TagToType[MeterAndTargeID_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        AT24CXX_Manager_NET.MeterID[0] = (((unsigned char)CmdSub.Name._char[5] != 0xFF) ? CmdSub.Name._char[5] : AT24CXX_Manager_NET.MeterID[0]);
        AT24CXX_Manager_NET.MeterID[1] = (((unsigned char)CmdSub.Name._char[6] != 0xFF) ? CmdSub.Name._char[6] : AT24CXX_Manager_NET.MeterID[1]);
        AT24CXX_Manager_NET.MeterID[2] = (((unsigned char)CmdSub.Name._char[7] != 0xFF) ? CmdSub.Name._char[7] : AT24CXX_Manager_NET.MeterID[2]);
        AT24CXX_Manager_NET.MeterID[3] = (((unsigned char)CmdSub.Name._char[8] != 0xFF) ? CmdSub.Name._char[8] : AT24CXX_Manager_NET.MeterID[3]);
        setIsWriteEEprom(true);
        ResFlag = true;
        LCD_MenuPage_Change('1'); // 跳转显示
    } else if (CmdSub.Name._char[3] == SetValve_TAG) { // 设定角度
        if (TagToType[SetValve_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        System_RunData.CtrlDev_Set_Degree_Part = U8_Connect_U8(CmdSub.Name._char[5], CmdSub.Name._char[6]);
        ResFlag = false;
    } else if (CmdSub.Name._char[3] == LimitTime_TAG) { // 截止日期
        if (TagToType[LimitTime_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        memcpy(&AT24CXX_Manager_NET.LimitTime, &CmdSub.Name._char[5], strlen("1999-05-20 13:14:00"));
        setIsWriteEEprom(true);
        ResFlag = true;
        LCD_MenuPage_Change('2'); // 跳转显示
        // 开启截至日期判断任务
        StartLimitCheckTask();
    } else if (CmdSub.Name._char[3] == FlagOfRights) { // 设置权限
        if (TagToType[FlagOfRights - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        AT24CXX_Manager_NET.ModeCode = U8_Connect_U8(CmdSub.Name._char[5], CmdSub.Name._char[6]);
        setIsWriteEEprom(true);
        ResFlag = true;
        LCD_MenuPage_Change('6'); // 跳转显示
        Reboot.EC20_ReBoot_In_RTC_By_1Second(&Reboot, 30);
    } else if (CmdSub.Name._char[3] == OpenValveRunTime_TAG) { // 设置累计开阀时间
        if (TagToType[OpenValveRunTime_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        // 累计开阀时间
        uint32_t RunTimeNum = 0;
        RunTimeNum = U8_Connect_U8(CmdSub.Name._char[5], CmdSub.Name._char[6]);
        RunTimeNum = RunTimeNum << 16;
        RunTimeNum = (RunTimeNum & 0xFFFF0000) | (U8_Connect_U8(CmdSub.Name._char[7], CmdSub.Name._char[8]) & 0x0000FFFF);
        EEprom_Write_Start((unsigned char *)&RunTimeNum, EEPROM_OTHER_ADDRESS + (AT24CXX_Manager_NET.RunTimeBack_AddrNum * 4), 4);
        ResFlag = false;
    } else if (CmdSub.Name._char[3] == RebootDev_TAG) { // 设置累计开阀时间
        if (TagToType[RebootDev_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        // 是否重启
        if (CmdSub.Name._char[5] == 0x01) {
            Reboot.EC20_ReBoot_In_RTC_By_1Second(&Reboot, 1);
        }
        ResFlag = false;
    } else if (CmdSub.Name._char[3] == TesTLCD_TAG) { // 设置累计开阀时间
        if (TagToType[TesTLCD_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        // 是否测试LCD
        if (CmdSub.Name._char[5] == 0x01) {
            RTC_TASK.CloseTask(HomePageRefresh);
            vTaskSuspendAll();  //阻止OS调度，防止打断
            TesTLCD();
            RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
            xTaskResumeAll();	//恢复OS调度
        }
        ResFlag = false;
    } else if (CmdSub.Name._char[3] == SendClassObj_TAG) {
        if (TagToType[SendClassObj_TAG - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        int NowAddr = 5;
        uint32_t Temp = 0;
        Temp |= CmdSub.Name._char[NowAddr++] << 24;
        Temp |= CmdSub.Name._char[NowAddr++] << 16;
        Temp |= CmdSub.Name._char[NowAddr++] << 8;
        Temp |= CmdSub.Name._char[NowAddr++];
        AT24CXX_Manager_NET.SendManageObj.SendIntervalDay = Temp;
        AT24CXX_Manager_NET.SendManageObj.SendStartHour = CmdSub.Name._char[NowAddr++];
        for (int i = 0; i < 4; i++) {
            Temp = 0;
            Temp |= CmdSub.Name._char[NowAddr++] << 8;
            Temp |= CmdSub.Name._char[NowAddr++];
            AT24CXX_Manager_NET.SendManageObj.SendFlagMode[i].StartDay = Temp;
            AT24CXX_Manager_NET.SendManageObj.SendFlagMode[i].SendInterval = CmdSub.Name._char[NowAddr++];
            AT24CXX_Manager_NET.SendManageObj.SendFlagMode[i].OneDayMaxSendTime = CmdSub.Name._char[NowAddr++];
        }
        setIsWriteEEprom((_Module_Start_Flage != 0 ? _Module_EEpromSend : true));
        ResFlag = true;
        goto SubEndAndWriteData;
    } else if (CmdSub.Name._char[3] == IPAndProt) {
        if (TagToType[IPAndProt - 1][1] != CmdSub.Name._char[2]) {
            ResFlag = false;
            goto SubEndAndWriteData;
        }
        char * LinkPoint = strstr(&CmdSub.Name._char[5], ":");
        (*LinkPoint) = 0;
        memcpy(AT24CXX_Manager_NET.NET_Remote_Url, &CmdSub.Name._char[5], strlen(&CmdSub.Name._char[5]));
        char ProtStr[6] = {0};
        memcpy(ProtStr, (LinkPoint + 1), (RXDataMaxLen - strlen(&CmdSub.Name._char[5]) - 6));
        AT24CXX_Manager_NET.NET_Remote_Port = doneAsciiStrToAnyBaseNumberData(ProtStr, 16);
        setIsWriteEEprom((_Module_Start_Flage != 0 ? _Module_EEpromSend : true));
        ResFlag = true;
        goto SubEndAndWriteData;
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

#include "TaskAboutTimer.h"
uint8_t HY_USB_TTL_CheckBuff(char * RxBuf, int RxLen, uint8_t Now_LPUartx) {
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
    int CsAddr = U8_Connect_U8(RXDatabuff.Name._char[9], RXDatabuff.Name._char[10]);
    if (CsAddr < 0 || CsAddr > RXDatabuff.MaxLen) {
        return false;
    }
    if ((RXDatabuff.Name._char[0] != 0x68) || ((RXDatabuff.Name._char[1] != 0x54))) {
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
    SendBuf = (Now_LPUartx == 0xF1 ? NEW_NAME(LPUART0Ddata.TxBuf) : NEW_NAME(CmdTable.TxBuf));
    memset(SendBuf.Name._char, 0, SendBuf.MaxLen);
    SendBuf.Name._char[SendDataLen++] = 0x68;
    SendBuf.Name._char[SendDataLen++] = 0x54;
    SendBuf.Name._char[SendDataLen++] = AT24CXX_Manager_NET.MeterID[0];
    SendBuf.Name._char[SendDataLen++] = AT24CXX_Manager_NET.MeterID[1];
    SendBuf.Name._char[SendDataLen++] = AT24CXX_Manager_NET.MeterID[2];
    SendBuf.Name._char[SendDataLen++] = AT24CXX_Manager_NET.MeterID[3];
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
        SendDataLen++; // 14
        SendDataLen++; // 15
        SendBuf.Name._char[SendDataLen++] = Type_ASCII; // 数据类型 TYPE
        SendBuf.Name._char[SendDataLen++] = upData_TAG; // 数据标记 TAG
        SendBuf.Name._char[SendDataLen++] = 0x01;   // VIF
        int temp = UpData_Receive_Hex(newJsonObjectByString(JsonInputStr));
        System_RunData.isUpCode = true;
        if (temp == 0) {
            SendBuf.Name._char[14] = 0x00;
            SendBuf.Name._char[15] = strlen("{\"upgrades\":\"start\"}") + 5;
            SendDataLen += catString(&SendBuf.Name._char[SendDataLen], "{\"upgrades\":\"start\"}", (512 - SendDataLen), strlen("{\"upgrades\":\"start\"}"));
            RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
        } else if (temp == 1) {
            newString(ResStr, 60);
            sprintf(ResStr.Name._char, "{\"NowPackNum\":%d,\"upgrades\":\"fail\"}", UpdataData.NowPackNum);
            SendBuf.Name._char[14] = 0x00;
            SendBuf.Name._char[15] = ResStr.getStrlen(&ResStr) + 5;
            SendDataLen += catString(&SendBuf.Name._char[SendDataLen], ResStr.Name._char, (512 - SendDataLen), ResStr.getStrlen(&ResStr));
        } else if (temp == 2) {
            newString(ResStr, 60);
            sprintf(ResStr.Name._char, "{\"NowPackNum\":%d,\"upgrades\":\"OK\"}", UpdataData.NowPackNum);
            SendBuf.Name._char[14] = 0x00;
            SendBuf.Name._char[15] = ResStr.getStrlen(&ResStr) + 5;
            SendDataLen += catString(&SendBuf.Name._char[SendDataLen], ResStr.Name._char, (512 - SendDataLen), ResStr.getStrlen(&ResStr));
        } else if (temp == 3) {
            SendBuf.Name._char[14] = 0x00;
            SendBuf.Name._char[15] = strlen("{\"upgrades\":\"over\"}") + 5;
            SendDataLen += catString(&SendBuf.Name._char[SendDataLen], "{\"upgrades\":\"over\"}", (512 - SendDataLen), strlen("{\"upgrades\":\"over\"}"));
            Reboot.EC20_ReBoot_In_RTC_By_1Second(&Reboot, 10);
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
#include "NetProt_Module.h"
        CmdTable.TxLen = ++SendDataLen;
    }
    if (isWirteEEprom == true) {
        return 0xFF;
    }
    return true;
}

#include "TaskGpioInit.h"
#include "../Interflow/Check_XTHF_Or_XTLF.h"
#include "ADC.h"
#include "GP21.h"
#include "NumberBaseLib.h"
#include "NB_Mode_Lib.h"
#include "MotorCtrlDev.h"
#include "CI522_interface.h"

// 测温设备初始化
void Check_Temper_Battery_Init(void) {
    MF_ADC_Common_Init();
    FL_GPIO_InitTypeDef GPIO_InitStruct;
    // PD4_GP21_PWR_EN 
    GPIO_InitStruct.pin = FL_GPIO_PIN_4;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    GP21_NTC_PWR_LOW;

    if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableADCTestTemper)) {
        // PC0_GP21_RST
        GPIO_InitStruct.pin = FL_GPIO_PIN_0 | FL_GPIO_PIN_1 | FL_GPIO_PIN_2 | FL_GPIO_PIN_9;
        GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
        GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.pull = FL_DISABLE;
        FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        GP21_RST_L;
        GP21_SOMI_L;
        GP21_SIMO_L;
        GP21_INT_L;

        MF_ADC_Sampling_Init(true);
    } else {
        MF_ADC_Sampling_Init(false);
        // PC6
        GPIO_InitStruct.pin = FL_GPIO_PIN_6;
        GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
        GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.pull = FL_DISABLE;
        FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GP21_GPIO_Init();
    }
}
void Device_Init(void) {
    MF_EXTI_Init();
    Config_Init_XTHF_And_XTLF();
    MF_CI522_Init();
    Check_Temper_Battery_Init();
    MF_Motor_Init();
    NB_Mode_Init();
    LPUART0_Init();    // 红外
    MF_LPUART0_Interrupt_Init();    // 开启 LPUart0
    CloseLPUart0TTL();
}
void StartOPenDevMode(void) {
    Device_Init();
    /* Initial IWDT */
    MF_IWDT_Init();
    MF_EXTI_Line_Init();        // 配置外部中断对应的端口
    // RTC 开启定时器中断
    MF_RTC_1S_Init();
    FL_RTC_InitTypeDef TempTime;
    ReadTime_EEprom(&TempTime);
    RTC_SetRTC(&TempTime);      // 设置时间结构体
    IncludeDelayMs(1000);
    RTC_TASK.InitSetTimeTask(IWDTClS, MinToSec(8), NULL); // 8min 内定时器喂狗
    // LPTIM 开启定时器中断
    // LPTIM_Setup();
    // LPTIM_Stop();
    // BSTIM 开启定时器中断
    // BSTIM32_Setup();         //开启定时器中断
    // BSTIM32_Stop();          //关闭定时器中断
#if( configUSE_TICKLESS_IDLE != 0 )
    // 关闭编程口, 改为控制 LED
    FL_GPIO_InitTypeDef GPIO_InitStruct;
    // PD7_LED_Open 灯  PD8_LEDClose 灯
    GPIO_InitStruct.pin = FL_GPIO_PIN_7 | FL_GPIO_PIN_8;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    LED_Open_LOW;
    LED_Close_LOW;
#endif
}
void CheckMeterNum(void) {
    uint32_t RunTimeNum = 0;
    EEprom_Read_Start((unsigned char *)&RunTimeNum, EEPROM_OTHER_ADDRESS + (AT24CXX_Manager_NET.RunTimeBack_AddrNum * 4), 4);
    if (RunTimeNum == 0xFFFFFFFF) {
        RunTimeNum = 0; // 初始化
        EEprom_Write_Start((unsigned char *)&RunTimeNum, EEPROM_OTHER_ADDRESS + (AT24CXX_Manager_NET.RunTimeBack_AddrNum * 4), 4);
    }
}

void SetErrerCode(CodeErr ErrId, bool UserSet) {
    System_RunData.StytemErrorCode = setDataBit(System_RunData.StytemErrorCode, ErrId, UserSet);
}

NetDevParameter System_RunData = {
    .CtrlDev_Set_Degree_Part = 0xFFFF,  // 预期阀门状态
    .Now_DEV_Volt = 3600,               // 当前电压
    .Now_Temper_T1 = 0,                 // 当前 T1
    .Now_Temper_T2 = 0,                 // 当前 T2
    .Now_Degree_Part = 0,               // 0 关 1000 开
    .TempRunTimeForValve = 0,           // 当前阀门大于 40% 的状态下的运行时间
    .FronValveDoneCode = 0,             // 上一次阀门动作的 ID
    .FrontValveDegree_Part = 0,         // 上一次阀门动作的角度
    .StytemErrorCode = 0,               // 系统错误码
    .ReceiveFlag = false,               // 上次通信是否接收到标记
    .isUpCode = false,                  // 是否升级
    .isPowerModuleUseing = false,
    .Now_NetDevParameter = {
        .isSendOk = false,
        .isWriteEEprom = false,
        .NowNetOnlineFlag = false,
        .isGetDownCmd = false,          // 是否定时主动获取指令（在模组没有提示字符时使用）
        .checkFlagStr = "+NNMI\r\n",    // 需要监控模组主动推出的提示字符串
    },
};

#include "StrLib.h"
#define IS_NVIC_VECTTAB(VECTTAB)  (((VECTTAB) == 0x00000000) || ((VECTTAB) == 0x08000000) || ((VECTTAB) == 0x20000000))
#define IS_NVIC_OFFSET(OFFSET)  (((OFFSET) & 0x1FFFFF80) == (OFFSET))
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset) {
    /*Check the parameters */
    assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
    assert_param(IS_NVIC_OFFSET(Offset));
    SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}

