
#ifndef _POWER_CTRL_H
#define _POWER_CTRL_H
#include "StrLib.h"

extern bool EnterLowPowerMode(void * InputConfig);
extern void ExitLowPowerMode(void * InputConfig);
extern bool _SetIsEnterLow_Power(bool SetBool, uint8_t UserCtrID);
// 使用屏蔽低功耗机制时，不能关闭调度器
#define SetIsEnterLow_Power(setbool, UserCtrID) \
do { \
    if (_SetIsEnterLow_Power(setbool, UserCtrID)) { \
        break; \
    } \
} while (1)
extern bool getIsEnterLow_Power(void);
#endif
