#include "PowerCtrl.h"
#include "RTC_SetTime.h"
#include "SetTime.h"
#include "BSTim.h"
#include "LPUart_0_And_1_Lib.h"
#include "StrLib.h"
#include "NumberBaseLib.h"

bool IsEnterLow_Power = false;
bool _SetIsEnterLow_Power(bool SetBool, uint8_t UserCtrID) {
    UserCtrID += 1;
    char UserID[2] = {0};
    doneBaseNumberDataToAsciiStr(UserID, 2, UserCtrID, 16);
    static char Front_UserCtrID[6] = {0};
    if (SetBool == false) {
        if (strlen(Front_UserCtrID) < 6) {
            catString(Front_UserCtrID, UserID, 6, 1);
            IsEnterLow_Power = false;
        }
        return true;
    }
    char * Addr_A = strchr(Front_UserCtrID, UserID[0]);
    char * Addr_B = Front_UserCtrID + strlen(Front_UserCtrID) - 1;
    if (Addr_A != NULL) {
        (*Addr_A) = '\0';
        swapChr(Addr_A, Addr_B);
    }
    if (strlen(Front_UserCtrID) == 0) {
        IsEnterLow_Power = true;
    }
    return true;
}
bool getIsEnterLow_Power(void) {
    return IsEnterLow_Power;
}
bool EnterLowPowerMode(void * InputConfig) {
    RTC_TASK.InitSetTimeTask(IWDTClS, MinToSec(8), NULL); // 8min 内定时器喂狗
    return IsEnterLow_Power;
}
void ExitLowPowerMode(void * InputConfig) {
    RTC_TASK.InitSetTimeTask(IWDTClS, MinToSec(8), NULL); // 8min 内定时器喂狗
    return;
}




