#ifndef __AT24C0XXX_H
#define __AT24C0XXX_H

#include <stdint.h>
#include <stdbool.h>
#include "RTC.h"
#include "Define.h"

#define EEProm_SDA_GPIO_X               GPIOA
#define EEProm_SDA_GPIO_PIN             FL_GPIO_PIN_12
#define EEprom_SDA_HIGH                 GPIO_SET_H(EEProm_SDA_GPIO_X, EEProm_SDA_GPIO_PIN)
#define EEprom_SDA_LOW                  GPIO_SET_L(EEProm_SDA_GPIO_X, EEProm_SDA_GPIO_PIN)  
#define EEprom_SDA_VALUE                (FL_GPIO_ReadInputPort(EEProm_SDA_GPIO_X)&EEProm_SDA_GPIO_PIN)
#define EEprom_SCL_HIGH                 GPIO_SET_H(GPIOA, FL_GPIO_PIN_11) 
#define EEprom_SCL_LOW                  GPIO_SET_L(GPIOA, FL_GPIO_PIN_11) 

typedef enum _ModeName {
    EnableADCTestTemper,            // P ADC测水温
    EnableGP2TestTemper,            // O GP2测水温
    ModeName_yuliu1,                // N 无线433使能
    EnableM1Card,                   // M M1刷卡使能
    EnableADCTestValve,             // L ADC测开启度使能（测限位开关是必须的，但是ADC测开启度是可选的）
    SaveRunTimeForOpenValve,        // K 保存开阀时间
    EnableTimeLimit,                // J 使能截止日期
    ValveAutoTest,                  // I 自检
    ModeName_yuliu2,                // H 外供电消失强制开阀
    ModeName_yuliu3,                // G 使能用户阀门控制权
    ModeName_yuliu4,                // F 用户可调范围
    ModeName_yuliu5,                // E 回温自动控制
    ModeName_yuliu6,                // D 回温梯度控制
    ModeName_yuliu7,                // C 强制开阀
    ModeName_yuliu8,                // B 使能测量电池电压
    Enable_NBMode,                  // A NB 模组使能
}ModeName_EN;

typedef struct AT24CXX_MANAGER_S {
    // 32字节
    char Sing;                              // 初始化标志
    char MeterID[4];                        // 表号
    uint16_t ModeCode;
    char LimitTime[30];
    uint16_t RunTimeBack_AddrNum;
    unsigned int OpenVOL;
    unsigned int CloseVOL;
    uint16_t UserSet_DegreePart;
    char NET_Remote_Url[42];    // 目标地址
    uint16_t NET_Remote_Port;   // 目标端口
    char DaysNumberOfCCLK;      // 间隔校时的天数
    struct {
        uint32_t SendIntervalDay;           // 当天发送的周期 (单位是 s) 1440 分钟 = 1天 = 86400 秒
        uint8_t  SendStartHour;             // 当天发送的起始点
        struct {
            uint16_t StartDay;              // 某模式起始日（如：第150日; 1月1日为第1日）
            uint8_t  SendInterval;          // 某模式下的间隔周期 (单位是 day)
            uint8_t  OneDayMaxSendTime;     // 某模式下的最大发送次数
        }SendFlagMode[4];                   // 当月发送标记模式 (默认：0:防锁模式, 1:频发模式, 2:保底模式)
    } SendManageObj;
    struct {
        char _Module_SIM_BAND;
        char _Module_SIM_ICCID[21];
        char _Module_SIM_IMSI[21];
        char _Module_DEV_IMEI[21];
        //char _Module_DEV_SNR;
        char _Module_DEV_CSQ;
        char _Module_DEV_RSRP;
        char _Module_DEV_RSRQ;
        char _Module_Socket_Num;
    } Get_Module_Data;
    FL_RTC_InitTypeDef Time_Data;
} AT24CXX_MANAGER_T;
extern AT24CXX_MANAGER_T AT24CXX_Manager;

extern int HY_MQTT_FLAG; // 1 汇云指令, 0 维泰指令
extern void SaveDevData(void);

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
#include "TaskEEprom.h"

#endif

