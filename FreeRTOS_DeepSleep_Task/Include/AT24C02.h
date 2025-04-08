#ifndef __AT24C0XXX_H
#define __AT24C0XXX_H

#include <stdint.h>
#include <stdbool.h>
#include "../Include/RTC.h"

typedef struct AT24CXX_MANAGER_S {
    // 32字节
    char Sing;                  // 初始化标志
    char MeterID[4];            // 表号
    uint8_t FrequencyPoint;     // 分频点
    uint16_t SendInterval;      // 发送间隔
    FL_RTC_InitTypeDef Time_Data;
} AT24CXX_MANAGER_T;
extern AT24CXX_MANAGER_T AT24CXX_Manager;

extern int HY_MQTT_FLAG; // 1 汇云指令, 0 维泰指令
// extern void SaveDevData(void);

#include "stdint.h"
#include <stdbool.h>

extern unsigned char EEprom_Byte1Read(unsigned char addr);
extern unsigned char EEprom_Byte1Write(unsigned char addr, unsigned char data);
extern unsigned int EEprom_Byte2Read(unsigned char addr);
extern void EEprom_Byte2Write(unsigned char addr, unsigned int data);

extern unsigned char EEprom_Write_Start(unsigned char * data, unsigned char addr, unsigned char len);
extern unsigned char EEprom_AT24CXX_Parameter_Init(bool isWriteFlag);
extern unsigned char EEprom_AT24C0XXData_Write(unsigned char * data, unsigned char len);   // 写参数
extern unsigned char EEprom_AT24C0XXData_Read(unsigned char * data, unsigned char size);
extern void ReadTime_EEprom(FL_RTC_InitTypeDef * SpaceBuff);

typedef struct {
    uint16_t SendCount;     // 发送次数
    uint16_t ReceiveCount;  // 接收次数
} NetDevParameter;

extern NetDevParameter Now_NetDevParameter;    // 网络状态标记与下行指令表
#endif

