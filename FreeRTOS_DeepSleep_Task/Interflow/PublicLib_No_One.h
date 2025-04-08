#ifndef __PUBLICLIB_NO_ONE_H
#define __PUBLICLIB_NO_ONE_H

// #define OPEN_AT_CMD_DEBUG
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
#include "../Include/Define.h"

#define IncludeDelayMs FL_DelayMs

#define ATCMD_MAXNUMBER 1
#define AT24CXX_Manager_NET (*_AT24CXX_Manager_NET)

extern void IPstrToHexArray(strnew IpHex, const char * Ipstr);
extern unsigned char get_CheckSum(unsigned char data[], int length);
// #define TimeSpeedNum 37 // 设置 1 秒等价与 TimeSpeetNum 秒
#ifdef TimeSpeedNum
extern void TimeSpeed(void);
#endif

extern void BCD_To_String(strnew str, strnew bcd);
extern void WGID_String_To_BCD(strnew bcd, strnew str);

// 开启测温，否则关闭测温开启测压
#define IsTemperature

#endif

