#ifndef NB_TXRX_H
#define NB_TXRX_H
#include "../Interflow/StrLib.h"

extern uint16_t _Module_TXD_6844_GetTx(void);
extern void _getHYCmdFormATStr(strnew Mather6844tr, uint16_t DataLen);
extern uint16_t PackHYDataToAtData(char * Databuf, uint16_t SendDataLen, strnew OutBuf);

#endif
