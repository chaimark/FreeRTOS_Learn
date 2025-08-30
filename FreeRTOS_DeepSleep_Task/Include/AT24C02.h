#ifndef __AT24C0XXX_H
#define __AT24C0XXX_H

#include <stdint.h>
#include <stdbool.h>
#include "RTC.h"
typedef enum _ModeName {
    EnableADCTestTemper,            // P ADC测水温
    EnableGP2TestTemper,            // O GP2测水温
    Enable433Open,                  // N 无线433使能
    EnableM1Card,                   // M M1刷卡使能
    EnableADCTestValve,             // L ADC测开启度使能（测限位开关是必须的，但是ADC测开启度是可选的）
    SaveRunTimeForOpenValve,        // K 保存开阀时间
    EnableTimeLimit,                // J 使能截止日期
    ValveAutoTest,                  // I 自检
    OpenValveForNoPowerOfExtern,    // H 外供电消失强制开阀
    EnableUserCtrlValve,            // G 使能用户阀门控制权
    UserCtrlAbleHeatRange,          // F 用户可调范围
    AotuCtrlT2,                     // E 回温自动控制
    SuperUserserOpenValve,          // D 强制开阀
    EnableTestBatV,                 // C 使能测量电池电压
    ModeName_yuliuB,
    ModeName_yuliuA,
}ModeName_EN;

typedef struct AT24CXX_MANAGER_S {
    // 32字节
    char Sing;                              // 初始化标志
    char MeterID[4];                        // 表号
    uint16_t ModeCode;
    uint8_t InsetT_Max;
    uint8_t InsetT_Min;
    char LimitTime[30];
    uint16_t RunTimeBack_AddrNum;
    struct {
        uint8_t T2Min;
        uint8_t T2Max;
        uint16_t T2_Set_Degree_Part;
    }T2AutoCtrlBuf[4];
    FL_RTC_InitTypeDef Time_Data;
} AT24CXX_MANAGER_T;
extern AT24CXX_MANAGER_T AT24CXX_Manager;

extern int HY_MQTT_FLAG; // 1 汇云指令, 0 维泰指令
// extern void SaveDevData(void);

#include "stdint.h"
#include <stdbool.h>
extern void MF_I2C_MASTER_Init(void);
extern unsigned char EEprom_Byte1Read(unsigned char addr);
extern unsigned char EEprom_Byte1Write(unsigned char addr, unsigned char data);
extern unsigned int EEprom_Byte2Read(unsigned char addr);
extern void EEprom_Byte2Write(unsigned char addr, unsigned int data);

extern unsigned char EEprom_Read_Start(unsigned char * Buff, unsigned short int addr, unsigned char len);
extern unsigned char EEprom_Write_Start(unsigned char * data, unsigned char addr, unsigned char len);
extern unsigned char EEprom_AT24CXX_Parameter_Init(bool isWriteFlag);
extern unsigned char EEprom_AT24C0XXData_Write(void * data, unsigned char len);   // 写参数
extern unsigned char EEprom_AT24C0XXData_Read(unsigned char * data, unsigned char size);
extern void ReadTime_EEprom(FL_RTC_InitTypeDef * SpaceBuff);

#endif

