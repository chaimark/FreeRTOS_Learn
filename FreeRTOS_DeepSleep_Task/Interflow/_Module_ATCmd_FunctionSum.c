#include "_Module_ATCmd_FunctionSum.h"
#include "NetProt_Module.h"
#include "NumberBaseLib.h"
#include "AT24CXXDataLoader.h"
#include "RTC_SetTime.h"
#include "Define.h"
#include "LPUart_0_And_1_Lib.h"
#include "HY_TXRX.h"
#include "NB_Mode_Lib.h"

void MF_AT_Module_Init(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct;
    //PB3 _Module_POWER_RES
    GPIO_InitStruct.pin = FL_GPIO_PIN_3;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    FL_GPIO_ResetOutputPin(GPIOB, FL_GPIO_PIN_3);
    //PC12__Module_PWR 
    GPIO_InitStruct.pin = FL_GPIO_PIN_12;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    FL_GPIO_ResetOutputPin(GPIOC, FL_GPIO_PIN_12);
}

// 发送报文
void sendDataBy_Module_Prot(char * SendCmd, int SendCmdLen) {
    ClearNetDataBuff();
    UART1_Send((unsigned char *)SendCmd, SendCmdLen);
}
// 定义AT指令
NetDevATCmd NetDevice_ATData__NB[ATCMD_MAXNUMBER] = {
    { 0, 1,"AT+IPR=115200\r\n","OK",true,3,2,1000,false,setUartBandDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},      //设置波特率
    { 1, 2,"AT+SGSW\r\n","OK",true,3,2,1000,false,setUartBandDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},
    { 2, 3,"AT+ECBAND=0,5\r\n","OK",true,3,2,1000,false,setSIMBandDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},       //设置 SIM BAND
    { 3, 4,"AT+CGSN\r\n","OK",true,3,2,1000,false,easyATCmdDataLoadFun,get_Module_DevIMEIFun, NULL, NULL},              //请求产品序列号IMEI
    { 4, 5,"AT+ECICCID\r\n","OK",true,3,2,1000,false,easyATCmdDataLoadFun,get_Module_DevCCIDFun, NULL, NULL},           //获取CCID
    { 5, 6,"AT+CPIN?\r\n","READY",true,3,2,2000,false,easyATCmdDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},          //识别sm卡
    { 6, 7,"AT+QSCLK=0\r\n","OK",true,3,2,1000,false,easyATCmdDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},           //关闭自动休眠锁
    { 7, 8,"AT+CSCON?\r\n","OK",true,3,2,1000,false,easyATCmdDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},            //信令连接状态
    { 8, 9,"AT+NNMI=2\r\n","OK",true,3,2,1000,false,easyATCmdDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},            //关闭下行数据回显
    { 9,10,"AT+CEREG?\r\n","CEREG: 0,1",true,10,1,1000,false,easyATCmdDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},
    {10,11,"AT+CFUN=1\r\n","OK",true,3,2,2000,false,easyATCmdDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},            //开启射频
    {11,12,"AT+CSQ\r\n","OK",true,3,2,3000,false,easyATCmdDataLoadFun,get_Module_DevCSQFun, NULL, NULL},                //获取信号质量
    {12,13,"AT+CESQ\r\n","OK",true,3,2,3000,false,easyATCmdDataLoadFun,get_Module_DevCESQFun, NULL, NULL},              //获得扩展的信号质量
    {13,14,"AT+CIMI\r\n","OK",true,3,2,1000,false,easyATCmdDataLoadFun,get_Module_DevIMSIFun, NULL, NULL},              //获得SIM卡的IMSI
    {14,15,"AT+CGPADDR?\r\n","OK",true,3,2,1000,false,easyATCmdDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},          //显示PDP地址
    {15,16,"AT+NCFG=0,86400\r\n","OK",true,3,2,8000,false,easyATCmdDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},      //如果对接 AEP 平台, 必须要配置AT+NCFG=0,<lifetime>, 否则无法连接上 AEP 平台
    {16,17,"AT+NCDPOPEN=\"221.229.214.202\",5683\r\n","QLWEVTIND: 3",true,3,2,3000,false,setRemoteIPDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},	//连接到目标IP
    {17,18,"AT+CCLK?\r\n","OK",true,3,2,1000,false,easyATCmdDataLoadFun,get_Module_DevCCLKFun, NULL, NULL},         //返回当前日期和时间
    {18,-1,"AT+NMGR\r\n","OK",false,3,2,5000,false,easyATCmdDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},         //获取下行数据
    {19,-1,"","",false,3,2,1000,false,easyATCmdDataLoadFun,easyATCmdDataDoingFun, NULL, NULL},
};

//======================================发送时的指令装载函数 ================================================//
void easyATCmdDataLoad(strnew OutStr, NetDevATCmd NowATCmd) {
    copyString(OutStr.Name._char, NowATCmd.ATCmd, OutStr.MaxLen, strlen(NowATCmd.ATCmd));
}
// 波特率设置特殊处理
void setDevBandCmdDataLoad(strnew OutStr, NetDevATCmd NowATCmd) {
    memset(OutStr.Name._char, 0, OutStr.MaxLen);
    sprintf(OutStr.Name._char, "AT+IPR=%d\r\n", UART_BAND);
}
// 设置 SIM BAND 
void setSIMBandCmdDataLoad(strnew OutStr, NetDevATCmd NowATCmd) {
    memset(OutStr.Name._char, 0, OutStr.MaxLen);
    // sprintf(OutStr.Name._char, "AT+ECBAND=0,%d\r\n", (AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_BAND != 0) ? AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_BAND : IMSI_BAND);
    sprintf(OutStr.Name._char, "AT+ECBAND=0,5\r\n"); // 5：中国电信 防止 eeprom 错误导致不上线
    int ArrayLen = strlen(OutStr.Name._char);
    OutStr.Name._char[(ArrayLen < OutStr.MaxLen) ? ArrayLen : (OutStr.MaxLen - 1)] = '\0';
}
// 目标IP特殊处理
void setRemoteIPCmdDataLoad(strnew OutStr, NetDevATCmd NowATCmd) {
    memset(OutStr.Name._char, 0, OutStr.MaxLen);
    sprintf(OutStr.Name._char, "AT+NCDPOPEN=\"%s\",%d\r\n", AT24CXX_Manager_NET.NET_Remote_Url, AT24CXX_Manager_NET.NET_Remote_Port);
    int ArrayLen = strlen(OutStr.Name._char);
    OutStr.Name._char[(ArrayLen < OutStr.MaxLen) ? ArrayLen : (OutStr.MaxLen - 1)] = '\0';
}
//========================接受模组反馈的信息, 判断是否发送成功, 或执行其他操作===================================//
bool easyATCmdDataDoing(NetDevATCmd NowATCmd) {
    if (myStrstr(CmdTable.DataBuff_RX, NowATCmd.SucessStr, CmdTable.NowRX_Len) != NULL)
        return true;
    else
        return false;
}
// 重启指令特殊处理
bool ReBootDevATCmdDoing(NetDevATCmd NowATCmd) {
    bool ReFalg = easyATCmdDataDoing(NowATCmd);
    IncludeDelayMs(1000); // 延时等待重启结束
    return ReFalg;
}
// 获取  CSQ
bool get_Module_DevCSQ(NetDevATCmd NowATCmd) {
    // +CSQ: 31,0
    char * StartAddr = NULL;
    if ((StartAddr = myStrstr(CmdTable.DataBuff_RX, "+CSQ: ", CmdTable.NowRX_Len)) != NULL) {
        StartAddr += strlen("+CSQ: ");
        char * EndAddr = NULL;
        if ((EndAddr = strchr(StartAddr, ',')) == NULL) {
            AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_CSQ = 0;
            return false;
        }
        (*EndAddr) = '\0';
        AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_CSQ = doneAsciiStrToAnyBaseNumberData(StartAddr, 16);
        return true;
    }
    AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_CSQ = 0;
    return false;
}
// 获取 CESQ
bool get_Module_DevCESQ(NetDevATCmd NowATCmd) {
    // +CESQ: 99,99,255,255,34,84 ==> <rsrq = 34>,<rsrp = 84>
    char * StartAddr = NULL;
    if ((StartAddr = myStrstrCont(CmdTable.DataBuff_RX, ",", CmdTable.NowRX_Len, 4)) != NULL) {
        StartAddr++;
        char * EndAddr = NULL;
        if ((EndAddr = strchr(StartAddr, ',')) == NULL) {
            (*EndAddr) = '\0';
        }
        AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_RSRQ = doneAsciiStrToAnyBaseNumberData(StartAddr, 16);

        StartAddr = EndAddr + 1;
        if ((EndAddr = strchr(StartAddr, '\r')) == NULL) {
            (*EndAddr) = '\0';
        }
        AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_RSRP = doneAsciiStrToAnyBaseNumberData(StartAddr, 16);
        return true;
    }
    AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_RSRQ = 0;
    AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_RSRP = 0;
    return false;
}
// 获取 IMEI
bool get_Module_DevIMEI(NetDevATCmd NowATCmd) {
    // 869254068477234
    char * StartAddr = NULL;
    if ((StartAddr = myStrstr(CmdTable.DataBuff_RX, "\r\n\r\nOK\r\n", CmdTable.NowRX_Len)) != NULL) {
        char * EndAddr = StartAddr;
        (*EndAddr) = '\0';
        StartAddr -= strlen("869254068477234");
        bool Temp_isWriteEEprom = false;
        // 比较 IMEI 是否相同
        Temp_isWriteEEprom |= ((strcmp(StartAddr, AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_IMEI) == 0) ? false : true);  // 如果相同则不写入
        strcpy(AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_IMEI, StartAddr);
        // 比较 ID 是否相同
        newString(TempID, 10);
        BCD_To_String(TempID, NEW_NAME(AT24CXX_Manager_NET.MeterID));
        StartAddr = StartAddr + strlen(AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_IMEI) - strlen("68477234");
        Temp_isWriteEEprom |= ((strcmp(StartAddr, TempID.Name._char) == 0) ? false : true);  // 如果相同则不写入
        if (Temp_isWriteEEprom) {    // 如果需要修改 IMEI 同步修改表号
            setIsWriteEEprom(true);
            EndAddr -= strlen("68477234");
            char MeterIDStr[10] = {0};
            strcpy(MeterIDStr, EndAddr);
            ASCIIToHEX2(MeterIDStr, 10, MeterIDStr, 10);
            memcpy(AT24CXX_Manager_NET.MeterID, MeterIDStr, 4);
        }
        return true;
    }
    memset(AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_IMEI, 0, ARR_SIZE(AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_IMEI));
    return false;
}
// 获取 IMSI
bool get_Module_DevIMSI(NetDevATCmd NowATCmd) {
    // 460113081449928
    char * StartAddr = NULL;
    if ((StartAddr = myStrstr(CmdTable.DataBuff_RX, "\r\n\r\nOK\r\n", CmdTable.NowRX_Len)) != NULL) {
        char * EndAddr = StartAddr;
        (*EndAddr) = '\0';
        StartAddr -= strlen("460113081449928");
        // 如果相同则不写入
        setIsWriteEEprom(((strcmp(StartAddr, AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_IMSI) == 0) ? 0xFF : true));  // 如果相同则不写入
        strcpy(AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_IMSI, StartAddr);
        return true;
    }
    memset(AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_IMSI, 0, ARR_SIZE(AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_IMSI));
    return false;
}
// 获取 CCID
bool get_Module_DevCCID(NetDevATCmd NowATCmd) {
    // +ECICCID: 89861122244006676924
    char * StartAddr = NULL;
    if ((StartAddr = myStrstr(CmdTable.DataBuff_RX, "+ECICCID: ", CmdTable.NowRX_Len)) != NULL) {
        StartAddr += strlen("+ECICCID: ");
        char * EndAddr = NULL;
        if ((EndAddr = strchr(StartAddr, '\r')) == NULL) {
            memset(AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_ICCID, 0, ARR_SIZE(AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_ICCID));
            return false;
        }
        (*EndAddr) = '\0';
        // 如果相同则不写入
        setIsWriteEEprom(((strcmp(StartAddr, AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_ICCID) == 0) ? 0xFF : true));
        strcpy(AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_ICCID, StartAddr);
        return true;
    }
    memset(AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_ICCID, 0, ARR_SIZE(AT24CXX_Manager_NET.Get_Module_Data._Module_SIM_ICCID));
    return false;
}
// CCLK 校时
bool get_Module_DevCCLK(NetDevATCmd NowATCmd) {
    if ((_Module_Start_Flage == _Module_UserSend) || (Min_15_TASK.Task[DayOverCclkTask].TimeTask_Falge)) {
        Min_15_TASK.InitSetTimeTask(DayOverCclkTask, HourToMin_15(AT24CXX_Manager_NET.DaysNumberOfCCLK * 24), NULL); // 604,800
    } else {
        return true;
    }

    bool isTimeOK = false;
    FL_RTC_InitTypeDef TempStringOfTime[2] = {0};
    for (int MaxCount_i = 0; MaxCount_i < 3; MaxCount_i++) { // 三次校时机会
        isTimeOK = false;
        ClearNetDataBuff();
        ////////////////////////////////////////////////
        for (uint8_t i = 0; i < 2; i++) {   // 任意一次两时间相同
            // 发送校时命令 "AT+CCLK?\r\n"
            sendDataByTTLProt("AT+CCLK?\r\n", strlen("AT+CCLK?\r\n"));
            for (int j = 0; j < 3; j++) {
                if (FindStr_WaitTime(NEW_NAME("+CCLK:"), 1000) == true) {
                    break;
                }
                ClearNetDataBuff();
            }

            // +CCLK: 2024/08/16,01:41:25+32
            char * StartAddr = NULL;
            FL_RTC_InitTypeDef _Module_Time_Data = {0};
            if ((StartAddr = myStrstr(CmdTable.DataBuff_RX, "+CCLK: ", CmdTable.NowRX_Len)) != NULL) {
                int Min15Cont = 0;
                sscanf(StartAddr, "+CCLK: %d/%d/%d,%d:%d:%d+%d", &_Module_Time_Data.year, &_Module_Time_Data.month, &_Module_Time_Data.day,
                    &_Module_Time_Data.hour, &_Module_Time_Data.minute, &_Module_Time_Data.second, &Min15Cont);
                _Module_Time_Data = RTCTime_Hex_AddSecToNewTime(_Module_Time_Data, (Min15Cont * 15 * 60));

                _Module_Time_Data.year = (uint32_t)anyBaseToAnyBase((uint64_t)_Module_Time_Data.year, 16, 10);     // 将 _Module_Time_Data.year 16进制转换成10进制
                _Module_Time_Data.month = (uint32_t)anyBaseToAnyBase((uint64_t)_Module_Time_Data.month, 16, 10);   // 将 _Module_Time_Data.month 16进制转换成10进制
                _Module_Time_Data.day = (uint32_t)anyBaseToAnyBase((uint64_t)_Module_Time_Data.day, 16, 10);       // 将 _Module_Time_Data.day 16进制转换成10进制
                _Module_Time_Data.hour = (uint32_t)anyBaseToAnyBase((uint64_t)_Module_Time_Data.hour, 16, 10);     // 将 _Module_Time_Data.hour 16进制转换成10进制
                _Module_Time_Data.minute = (uint32_t)anyBaseToAnyBase((uint64_t)_Module_Time_Data.minute, 16, 10); // 将 _Module_Time_Data.minute 16进制转换成10进制
                _Module_Time_Data.second = (uint32_t)anyBaseToAnyBase((uint64_t)_Module_Time_Data.second, 16, 10); // 将 _Module_Time_Data.second 16进制转换成10进制
                _Module_Time_Data.week = getDayOfWeek(_Module_Time_Data.year, _Module_Time_Data.month, _Module_Time_Data.day);

                if (!checkTimeFrom(_Module_Time_Data) || AT24CXX_Manager_NET.Get_Module_Data._Module_DEV_CSQ < 5) {
                    return false;    // 时间错误
                }
                TempStringOfTime[i] = _Module_Time_Data;
            }
        }
        ////////////////////////////////////////////////
        if (TempStringOfTime[0].year != TempStringOfTime[1].year) {
            continue;
        }
        if (TempStringOfTime[0].month != TempStringOfTime[1].month) {
            continue;
        }
        if (TempStringOfTime[0].day != TempStringOfTime[1].day) {
            continue;
        }
        if (TempStringOfTime[0].hour != TempStringOfTime[1].hour) {
            continue;
        }
        if (TempStringOfTime[0].minute != TempStringOfTime[1].minute) {
            continue;
        }
        isTimeOK = true;
        break;
    }
    if (isTimeOK) {
        char setStrOfTime[32] = {0};
        strnew NowTimeData = NEW_NAME(setStrOfTime);
        sprintf(NowTimeData.Name._char, "%02x-%02x-%02x %02x:%02x:%02x", TempStringOfTime[1].year, TempStringOfTime[1].month, TempStringOfTime[1].day, TempStringOfTime[1].hour, TempStringOfTime[1].minute, TempStringOfTime[1].second);
        setRtcDate(NowTimeData, false); // 设置时间
        setIsWriteEEprom(_Module_EEpromTime);
        return true;
    } else {
        return false;
    }
}

// 设置或查询 AT指令 的装载方法
void DataInstallation__NB(strnew OutStr, NetDevATCmd This) {
    // 调用实际装载的函数
    switch (This.LoadATSendFunName) {
        case easyATCmdDataLoadFun:
            easyATCmdDataLoad(OutStr, This);
            break;
        case setUartBandDataLoadFun:
            setDevBandCmdDataLoad(OutStr, This);
            break;
        case setSIMBandDataLoadFun:
            setSIMBandCmdDataLoad(OutStr, This);
            break;
        case setRemoteIPDataLoadFun:
            setRemoteIPCmdDataLoad(OutStr, This);
            break;
    }
}
// 处理指令返回的方法
bool DoingATCmdResData__NB(NetDevATCmd This) {
    bool ResFlag = false;
    // 调用实际处理回复的函数
    switch (This.DoingATReceiveFunName) {
        case ResetTrueFlagDoingFun:
            ResFlag = true;
            break;
        case easyATCmdDataDoingFun:
            ResFlag = easyATCmdDataDoing(This);
            break;
        case ReBootDevFun:
            ResFlag = ReBootDevATCmdDoing(This);
            break;
        case get_Module_DevIMEIFun:
            ResFlag = get_Module_DevIMEI(This);
            break;
        case get_Module_DevCSQFun:
            ResFlag = get_Module_DevCSQ(This);
            break;
        case get_Module_DevCESQFun:
            ResFlag = get_Module_DevCESQ(This);
            break;
        case get_Module_DevIMSIFun:
            ResFlag = get_Module_DevIMSI(This);
            break;
        case get_Module_DevCCIDFun:
            ResFlag = get_Module_DevCCID(This);
            break;
        case get_Module_DevCCLKFun:
            ResFlag = get_Module_DevCCLK(This);
            break;
    }
    return ResFlag;
}
bool isMQTTLinkOnleng__NB(void) {
    return true;
}
bool EnterATMode__NB(bool isResDev) {
    if (!isResDev) {
        return true;
    }
    // 供电
    NB_PWR_LOW;
    IncludeDelayMs(100);
    MF_UART1_Init();
    MF_UART1_Interrupt_Init();
    NB_PWR_HIGH;
    IncludeDelayMs(200);
    // 开机
    NB_PWR_LOW;
    IncludeDelayMs(1000);
    NB_PWR_HIGH;
    IncludeDelayMs(3000);
    NB_PWR_LOW;
    return true;
}
void Shutdown_Module_(void) {
    NB_PWR_LOW; // 关电
    // PC2:_Module_RX PC3:_Module_X
    CloseUart1TTL();
    IncludeDelayMs(100);
}
/////////////////////////////////////
// 发送内存队列中保存的数据 （接口由 模组层调用 并 上浮到 RTOS_Task 层）
bool _Module_Send_Data(void) {
    bool ResFlag = false;
    int StrLen = _Module_TXD_6844_GetTx();
    sendDataByTTLProt(UART1Ddata.TxBuf, StrLen);
    memset(UART1Ddata.TxBuf, 0, ARR_SIZE(UART1Ddata.TxBuf));
    for (int i = 0; i < 3; i++) {
        if (FindStr_WaitTime(NEW_NAME("+QLWEVTIND:"), 1000) == false) {
            ClearNetDataBuff();
            continue;
        }
        ResFlag = true;
        break;
    }
    if (ResFlag == false) {  // 发送失败, 直接退出
        return ResFlag;
    }
    for (int i = 0; (i < 6 && Meter_Manager.DataCount > 0); i++) {
        Meter_Manager.DeleteQueue(&Meter_Manager);  // delete queue
    }
    return ResFlag;
}
void UserGetDownCmd(void) {
    sendDataBy_Module_Prot("AT+NMGR\r\n", strlen("AT+NMGR\r\n"));    // 获取下行指令
}
bool UserSendData(void) {
    bool SendFlag = false;
    for (int i = 0; i < 3; i++) {
        if ((SendFlag = _Module_Send_Data()) == true) {
            i = 0;
        } else {
            continue;
        }
        if (Meter_Manager.DataCount <= 0) {
            break;
        }
    }
    return SendFlag;
}
// 监控字符串特殊处理含糊
void SpecialDone_NB(void) {
    memset(CmdTable.DataBuff_RX, 0, (CmdStrLenMax + 1));
    sendDataBy_Module_Prot("AT+NMGR\r\n", strlen("AT+NMGR\r\n"));    // 获取下行指令
    return;
}

