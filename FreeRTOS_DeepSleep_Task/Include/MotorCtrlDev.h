
#ifndef _MOTOR_CTRL_DEV_H
#define _MOTOR_CTRL_DEV_H
#include "PublicLib_No_One.h"

typedef enum _ValveReasonsDone {
    VALVE_REASON_RETURN_WATER_TEMP,     // P 回水温度
    VALVE_REASON_POWER_OFF_OPEN,        // O 断电开阀
    VALVE_REASON_SELF_CHECK,            // N 自检
    VALVE_REASON_WIRELESS_THERMOSTAT,   // M 无线温控器
    VALVE_REASON_CARD_SWIPE,            // L 刷卡
    VALVE_REASON_SYSTEM_FORCE_OPEN,     // K 系统强制开阀
    VALVE_REASON_SYSTEM_DEADLINE,       // J 系统截止日期
    VALVE_REASON_MBUS_INTERFACE,        // I MBUS接口
    VALVE_REASON_INFRARED_INTERFACE,    // H 红外接口
    VALVE_REASON_NB_MQTT_ONENET_RESERVE // G NB MQTT OneNet接口预留
}ValveReasonsDone;

extern void MF_Motor_Init(void);
extern void Valve_SetDegree(uint16_t Set_Degree_Part, ValveReasonsDone DoneId);
#endif
