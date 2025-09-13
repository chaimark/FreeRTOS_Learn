#include "NetProt_Module.h"
#include "NumberBaseLib.h"
#include "StrLib.h"
#include "SetTime.h"
#include "RTC_SetTime.h"
#include "Define.h"
#include "HY_TXRX.h"
#include "_Module_ATCmd_FunctionSum.h"
#include "Display.h"

/******************************/
NetDevATCmd * NetDevice_ATData;
static strnew UartBuff = {0};    // Uart接收缓冲区

bool checkUart(int MaxTimeMs) {
    int MaxTime = (MaxTimeMs == 0 ? 1000 : MaxTimeMs);
    bool Flag = false;
    for (int i = 0; i < MaxTime; i++) {
        Flag = (CmdTable.RxLen > 0 ? true : false);
        if (Flag) {
            break;
        }
        IncludeDelayMs(1);
    }
    return Flag;
}
bool _FindStr_WaitTime(strnew FindStr, int MaxTimeMs, char ** OutNote) {
    char * TempP = NULL;
    if (checkUart(MaxTimeMs)) {
        if ((TempP = strstr(CmdTable.RxBuf, FindStr.Name._char)) == NULL) {
            return false;
        }
        (*OutNote) = ((OutNote != NULL) ? TempP : NULL);
        return true;
    }
    return false;
}
void _SpecialDoneFromTask(uint8_t Special_ID) {
#ifdef IsShortLinkModeFlag
    Task_SpecialDone_NB(Special_ID);
#else
    if (UP_Mode_EC20_ON) {
#ifdef _4G_ATCMD_FUNCTIONSUM_H
#endif
    } else {
#ifdef _NET_ATCMD_FUNCTIONSUM_H
#endif
    }
#endif
}
void _SpecialDoneFromISR(uint8_t Special_ID) {
#ifdef IsShortLinkModeFlag
    ISR_SpecialDone_NB(Special_ID);
#else
    if (UP_Mode_EC20_ON) {
#ifdef _4G_ATCMD_FUNCTIONSUM_H
#endif
    } else {
#ifdef _NET_ATCMD_FUNCTIONSUM_H
#endif
    }
#endif
}
void clearUartBuff(void) {
    memset(UartBuff.Name._char, 0, UartBuff.MaxLen);
    NowLenOfUartBuff = 0;   // 需要把外部传入的buff标记或长度复位才算清空
}
void ClearNetDataBuff(void) {
    memset(CmdTable.RxBuf, 0, (LONG_UARTMAX + 1));
    CmdTable.RxLen = 0;
}
////////////////////////////////////////////////////////
void copyDataFormUART(void) {
    if (NowLenOfUartBuff != 0) {
        if (NowLenOfUartBuff > LONG_UARTMAX) {
            clearUartBuff();
            return;
        }
        if (CmdTable.RxLen + NowLenOfUartBuff <= (LONG_UARTMAX + 1)) {  // 追加到 CmdTable.
            memcpy(&CmdTable.RxBuf[CmdTable.RxLen], UartBuff.Name._char, NowLenOfUartBuff); // 接收数据
            CmdTable.RxLen += NowLenOfUartBuff;
        } else {
            ClearNetDataBuff();
            memcpy(CmdTable.RxBuf, UartBuff.Name._char, NowLenOfUartBuff);   // 接收数据
            CmdTable.RxLen = NowLenOfUartBuff;
        }
        clearUartBuff();
    }
}
// copy Buf 到 CmdTale
void getHYCmdFormATStr(strnew MothreBuf, uint16_t DataLen) {
    _getHYCmdFormATStr(MothreBuf, DataLen);
}
////////////////////////////////////////////////////////
void sendDataByTTLProt(void * dataStr, int SendCmdLen) {
    char * SendCmd = (char *)dataStr;
    sendDataBy_Module_Prot(SendCmd, SendCmdLen);
}
// 设置模组处于AT模式
bool SetDevATCMDModel_ThroughSendData(void) {
    checkUart(0);
    bool ATConfig_Flag;
    setIsWriteEEprom(false);
#ifdef IsShortLinkModeFlag
    ATConfig_Flag = EnterATMode__NB((System_RunData.Now_NetDevParameter.NowNetOnlineFlag ? false : true));
#else
//NET 进入 AT 模式
    if (UP_Mode_EC20_ON) {
#ifdef _4G_ATCMD_FUNCTIONSUM_H
        ATConfig_Flag = EnterATMode_4G((System_RunData.Now_NetDevParameter.NowNetOnlineFlag ? false : true));
#endif
    } else {
#ifdef _NET_ATCMD_FUNCTIONSUM_H
        ATConfig_Flag = EnterATMode_NET((System_RunData.Now_NetDevParameter.NowNetOnlineFlag ? false : true));
#endif
    }
#endif

#ifdef OPEN_AT_CMD_DEBUG
    ATCmdDebugTask();
#ifdef IsShortLinkModeFlag
    RTC_TASK.InitSetTimeTask(SendTimeOverTask, 1, NULL); // 超过 1 s, 需要关闭 _Module_
#endif
#endif
    System_RunData.Now_NetDevParameter.isSendOk = false;
    setIsWriteEEprom(false);
    // 进入AT模式 ==========
    return ATConfig_Flag;
}
// 处理用户请求/命令
bool doingUserRequest(void) {
    return true;
}
// 发送ATCmd
bool sendATCmdData(NetDevATCmd NowATCmd) {
    newString(ATCmd_SendBUFF, 500);
    // 装载指令
    NowATCmd.DataInstallation(ATCmd_SendBUFF, &NowATCmd);
    clearUartBuff();
    // 发送指令
    sendDataByTTLProt(ATCmd_SendBUFF.Name._char, ATCmd_SendBUFF.getStrlen(&ATCmd_SendBUFF));
    for (int ResCount_i = 0; ResCount_i <= NowATCmd.CmsResCount; ResCount_i++) {
        newstrobj(TempSucessStr, 1);
        TempSucessStr.Name._char = NowATCmd.SucessStr;
        TempSucessStr.MaxLen = TempSucessStr.getStrlen(&TempSucessStr);
        if (FindStr_WaitTime(TempSucessStr, NowATCmd.DelayTime) == false) {   // 模组是否回复
            ClearNetDataBuff();
            // 判断是否可以跳过
            if ((NowATCmd.DoingATReceiveFunName == ResetTrueDoingFun) && ((ResCount_i + 1) == NowATCmd.CmsResCount)) {
                return true; // 发送结束也没收到回复, 直接跳过
            }
            continue;
        }
        // 处理模组的回复
        if (NowATCmd.DoingATCmdResData(&NowATCmd) == false) {
            return false; // AT 指令发送失败
        }
        // 该指令是否需要上位机回复, 如果不需要直接回复 true
        if (NowATCmd.IsGetComputerResFlag == false) {
            return true; // 上位机回复成功
        }
        return doingUserRequest(); // 上位机回复成功
    }
    return (NowATCmd.CmsResCount == 0 ? true : false); // AT 指令发送失败限制为0,直接跳过该指令
}
bool is_Module_OverTime(bool isStart) {
    static TickType_t StartRTC_Time = 0;
    if (isStart) {
        StartRTC_Time = xTaskGetTickCount();
    }
    TickType_t NowRTC_Time = xTaskGetTickCount();
    if ((NowRTC_Time - StartRTC_Time) > (MinToSec(2) * 1000)) {
        return true;
    }
    return false;
}
// 判断是否连接上MQTT （参数 ：false 不重启模组, true 设备不在线则重启模组）
bool isMQTTLinkOnleng(bool isResAtDev) {
    bool ATConfig_Flag = false;
    if (isResAtDev) {
        for (int i = 0; i < 10; i++) {
            if (SetDevATCMDModel_ThroughSendData()) {
                break;
            }
        }
    }
#ifdef IsShortLinkModeFlag
    ATConfig_Flag = isMQTTLinkOnleng__NB();
#else
    if (UP_Mode_EC20_ON) {
#ifdef _4G_ATCMD_FUNCTIONSUM_H
        ATConfig_Flag = isMQTTLinkOnleng_4G();
#endif
    } else {
#ifdef _NET_ATCMD_FUNCTIONSUM_H
        ATConfig_Flag = isMQTTLinkOnleng_NET();
#endif
    }
#endif
    if (ATConfig_Flag) {
        return true;
    }
    return false;
}
void Net_Task(void) {
    is_Module_OverTime(true);
    NetAT_Init();
    // 设置模组进入 AT 模式
    for (int i = 0; i < 10; i++) {
        if (SetDevATCMDModel_ThroughSendData()) {
            if (is_Module_OverTime(false)) {
                return;
            }
            break;
        }
    }
    int NowStep = 0;
    while (NowStep != -1) {
        // 一个是否执行的开关, 后续可扩展为指令控制
        if (NetDevice_ATData[NowStep].RunFlag) {
            for (int SendCount_i = 0; SendCount_i < NetDevice_ATData[NowStep].CmdSendCount; SendCount_i++) {
                if (sendATCmdData(NetDevice_ATData[NowStep])) {
                    Display__Module_Steep((unsigned char)NowStep % 100, 1);
                    break;
                }
                Display__Module_Steep((unsigned char)NowStep % 100, 0);
                // 联网遇到问题, 中断联网进程
                if ((SendCount_i + 1) == NetDevice_ATData[NowStep].CmdSendCount) {
                    IncludeDelayMs(500);   // 连接流程结束后等待 2 秒	
                    Display__Module_Steep(255, 0);
                    IncludeDelayMs(1500);
                    System_RunData.Now_NetDevParameter.NowNetOnlineFlag = false; // 标记不在线, 下次运行这个任务时重启
                    // printf("The network connection has been interrupted : %s\r\n", NetDevice_ATData[NowStep].ATCmd);
                    return;
                }
            }
        }
        NowStep = NetDevice_ATData[NowStep].Next_CmdID;
        if (is_Module_OverTime(false)) {
            return;
        }
    }
    IncludeDelayMs(3000);
    // 判断是否在线
    if (isMQTTLinkOnleng(false)) {
        IncludeDelayMs(50);
        Display__Module_Steep(255, 1);
        IncludeDelayMs(1500);
        System_RunData.Now_NetDevParameter.NowNetOnlineFlag = true;
    }
    if (is_Module_OverTime(false)) {
        return;
    }
    ClearNetDataBuff();
    if (System_RunData.Now_NetDevParameter.SendData != NULL) {
        System_RunData.Now_NetDevParameter.isSendOk = System_RunData.Now_NetDevParameter.SendData();  // 发送数据
        ClearNetDataBuff(); // 清空数据
    }
    return;
}

/***********************************************/
/***********************************************/
/***********************************************/
// 设置或查询 AT指令 的装载方法
void _DataInstallation(strnew OutStr, struct _NetDevATCmdData This) {
    DataInstallation__NB(OutStr, &This);
}
// 处理指令返回的方法
bool _DoingATCmdResData(struct _NetDevATCmdData This) {
    bool ResFlag = false;
    ResFlag = DoingATCmdResData__NB(&This);
    return ResFlag;
}
// 建立对象的函数
NetDevATCmd New_NetDevAT_Obj(NetDevATCmd DataInit) {
    DataInit.DataInstallation = _DataInstallation;
    DataInit.DoingATCmdResData = _DoingATCmdResData;
    return DataInit;
}

void NetAT_Init(void) {
    // 初始化 NetDevice_ATData
    NetDevice_ATData = NetDevice_ATData__NB;
    for (int i = 0; i < ATCMD_MAXNUMBER; i++) {
        NetDevice_ATData[i] = New_NetDevAT_Obj(NetDevice_ATData[i]);
    }
    System_RunData.Now_NetDevParameter.GetDownCmd = UserGetDownCmd;  // 获取下行指令
    System_RunData.Now_NetDevParameter.SendData = UserSendData;      // 长连接不需要发送函数
    System_RunData.Now_NetDevParameter.UartCmdType = UartCmdType;
    UartBuff = NEW_NAME(UART_DATABUFF);
    return;
}


