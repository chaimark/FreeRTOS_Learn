#ifndef WEB_NET_PROT_H
#define WEB_NET_PROT_H

#include <stdint.h>
#include <stdbool.h>
#include "StrLib.h"
#include "PublicLib_No_One.h"
#include "AT24CXXDataLoader.h"
// 外部引入
#define UartDisableIRQ          NVIC_DisableIRQ(UART0_IRQn) // 关闭中断使能
#define UartEnableIRQ           NVIC_EnableIRQ(UART0_IRQn)  // 开启中断使能
#define UART_DATABUFF           UART1Ddata.RxBuf            // 接收缓冲区
#define NowLenOfUartBuff        UART1Ddata.RxLen            // 接收缓冲区长度

// 内部定义
#define IsShortLinkModeFlag             // 是否是短连接标记
#define CmdTable LPUART0Ddata
// 公共接口
extern void Net_Task(void);
extern void ClearNetDataBuff(void);
extern void copyDataFormUART(void);
extern void getHYCmdFormATStr(strnew MothreBuf, uint16_t DataLen);
extern void sendDataByTTLProt(void * dataStr, int SendCmdLen);
////////////////////////////////////////////////
#define ResetTrueDoingFun 0
// 类定义
typedef struct _NetDevATCmdData {   // 成员
    int8_t CmdID;         // 指令ID
    int8_t Next_CmdID;    // 下一条指令ID
    char * ATCmd;         // AT指令
    char * SucessStr;     // 成功返回的字符串
    bool RunFlag;         // 是否执行当前指令

    uint8_t CmdSendCount; // 指令发送次数限制
    uint8_t CmsResCount;  // 回复查询次数限制
    uint16_t DelayTime;   // 单次最大等待时间

    bool IsGetComputerResFlag; // 是否需要获取电脑返回结果

    uint8_t LoadATSendFunName;      // 装载数据的函数名
    uint8_t DoingATReceiveFunName;  // 处理返回结果的函数名

    void (*DataInstallation)(strnew OutStr, struct _NetDevATCmdData This);
    bool (*DoingATCmdResData)(struct _NetDevATCmdData This);
} NetDevATCmd;
// 类方法
void _DataInstallation(strnew OutStr, struct _NetDevATCmdData This); // 设置或查询 AT指令 的装载方法
bool _DoingATCmdResData(struct _NetDevATCmdData This);               // 处理指令返回的方法
extern NetDevATCmd * NetDevice_ATData;
extern void NetAT_Init(void);

extern bool _FindStr_WaitTime(strnew FindStr, int MaxTimeMs, char ** OutNote);
#define _FindStr_WaitTime2(str, timeMax)  _FindStr_WaitTime(str, timeMax, NULL)
#define _FindStr_WaitTime3(str, timeMax, Prt) _FindStr_WaitTime(str, timeMax, Prt)

#define GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define FindStr_WaitTime(...) GET_MACRO(__VA_ARGS__, _FindStr_WaitTime3, _FindStr_WaitTime2)(__VA_ARGS__)

// 监控到某字符, 特殊处理, 不同模组处理不同, 独立实现 API.
extern void _SpecialDoneFromTask(uint8_t Special_ID);
#define SpecialDoneFromTask _SpecialDoneFromTask

extern void _SpecialDoneFromISR(uint8_t Special_ID);
#define SpecialDoneFromISR _SpecialDoneFromISR

#endif


