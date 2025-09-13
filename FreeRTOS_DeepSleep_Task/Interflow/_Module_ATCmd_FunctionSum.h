#ifndef ATCMD_FUNCTIONSUM_H
#define ATCMD_FUNCTIONSUM_H

#include "PublicLib_No_One.h"
#include <stdint.h>
#include "StrLib.h"
#include "NetProt_Module.h"
extern void MF_AT_Module_Init(void);
extern void Shutdown_Module_(void);
extern void sendDataBy_Module_Prot(char * txBuffer, int length);
#define IncludeUART0_4G_BAND UART0_BAND   // 需要引入外部变量或者常量

#define This (*this)
// 所有 AT指令 《发送时》 需要的用于 《组织指令》 的函数名
#define easyATCmdDataLoadFun 0           /* 简单AT指令装载 */
#define setUartBandDataLoadFun 1         /* 波特率装载设置 */
#define setSIMBandDataLoadFun 2          /* 波特率装载设置 */
#define setRemoteIPDataLoadFun 3         /* 目标IP设置 */
// 所有 AT指令 《回复时》 需要的用于 《处理回复》 的函数名
#define ResetTrueFlagDoingFun 0     /* 作为特殊标记, 用于忽略模组返回强制跳过某条指令 */
#define easyATCmdDataDoingFun 1     /* 简单AT指令处理 */
#define ReBootDevFun 2              /* 重启命令特殊处理 */
#define get_Module_DevCSQFun 3      /* 获取信号强度 */
#define get_Module_DevIMEIFun 4     /* 获取IMEI */
#define get_Module_DevCCIDFun 5     /* 获取CCID */
#define get_Module_DevCCLKFun 6     /* 校时机制 */
#define get_Module_DevIMSIFun 7     /* 获取IMSI */
#define get_Module_DevCESQFun 8     /* 获取扩展信号强度 */
#define get_Module_IPLinkFun  9     /* 等待完全连网成功 */

extern NetDevATCmd NetDevice_ATData__NB[ATCMD_MAXNUMBER];
extern void DataInstallation__NB(strnew OutStr, struct _NetDevATCmdData This);
extern bool DoingATCmdResData__NB(struct _NetDevATCmdData This);
extern bool isMQTTLinkOnleng__NB(void);
extern bool EnterATMode__NB(bool isResDev);
extern void Task_SpecialDone_NB(uint8_t Special_ID);
extern void ISR_SpecialDone_NB(uint8_t Special_ID);
/////////////////////////////////////
extern void UserGetDownCmd(void);
extern bool UserSendData(void);
extern uint8_t UartCmdType(void);
#endif
