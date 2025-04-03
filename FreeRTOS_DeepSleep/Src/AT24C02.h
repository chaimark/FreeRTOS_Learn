#ifndef __AT24C0XXX_H
#define __AT24C0XXX_H

#include <stdint.h>
#include <stdbool.h>
#include "../Interflow/RTC.h"

#define RunModeOfSystemStart 1
#define ModeMax 6
typedef struct _ModeForTime {
    uint32_t StartPointOfNowSetTemper;  // 当前时段开始的时间点 单位分钟
    float InsideTSetOfNowMode;          // 当前模式的设定温度
}ModeForTime;
extern ModeForTime ModeFor_Now_InsideTSet[ModeMax];

typedef struct AT24CXX_MANAGER_S {
    // 32字节
    char Sing;                  // 初始化标志
    char TargetDevID[4];        // 目标设备 ID
    char MeterID[4];            // 表号
    uint8_t NowRunMode;         // 当前运行模式  0：关机 1：自动（默认模式） 2：速热 3：时段 4：防冻
    uint8_t UserModeFlag;       // 用户模式标志, 0: 未设置(读 eeprom 不覆盖) 1: 设置(读 eeprom 覆盖)
    char DaysNumberOfCCLK;      // 间隔校时的天数
    ModeForTime ModeFor_1_5_InsideTSet[ModeMax];
    ModeForTime ModeFor_6_7_InsideTSet[ModeMax];
    FL_RTC_InitTypeDef Time_Data;
} AT24CXX_MANAGER_T;
extern AT24CXX_MANAGER_T AT24CXX_Manager;

extern int HY_MQTT_FLAG; // 1 汇云指令, 0 维泰指令
extern void SaveDevData(void);
extern void changeModeForT(void);

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

#endif

