#ifndef __PUBLICLIB_NO_ONE_H
#define __PUBLICLIB_NO_ONE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "fm33lc0xx_fl.h"
#include "StrLib.h"
#include "Define.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Tickless_Hook.h" 
#include "semphr.h"
#include "timers.h"

extern void FL_DelayUs(uint32_t nus);
#define IncludeDelayMs FL_DelayMs

#define ATCMD_MAXNUMBER 20
#define AT24CXX_Manager_NET (*_AT24CXX_Manager_NET)

#define _Module_UserSend    60   // 最大不能超过 255
#define _Module_AutoSend    61   // 最大不能超过 255
#define _Module_EEpromSend  62   // 最大不能超过 255
#define _Module_EEpromTime  63   // 最大不能超过 255

extern void IPstrToHexArray(strnew IpHex, const char * Ipstr);
extern unsigned char get_CheckSum(unsigned char data[], int length);
// #define TimeSpeedNum 37 // 设置 1 秒等价与 TimeSpeetNum 秒
#ifdef TimeSpeedNum
extern void TimeSpeed(void);
#endif

extern void BCD_To_String(strnew str, strnew bcd);
extern void WGID_String_To_BCD(strnew bcd, strnew str);

//////////////////////////////////////////////////////



#endif

