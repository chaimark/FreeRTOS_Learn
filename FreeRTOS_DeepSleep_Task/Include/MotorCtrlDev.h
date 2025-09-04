
#ifndef _MOTOR_CTRL_DEV_H
#define _MOTOR_CTRL_DEV_H
#include "PublicLib_No_One.h"
#include "Define.h"
#define BL113_IN1_HIGH                  GPIO_SET_H(GPIOB, FL_GPIO_PIN_2)
#define BL113_IN1_LOW                   GPIO_SET_L(GPIOB, FL_GPIO_PIN_2) 
#define BL113_IN2_HIGH                  GPIO_SET_H(GPIOB, FL_GPIO_PIN_3)
#define BL113_IN2_LOW                   GPIO_SET_L(GPIOB, FL_GPIO_PIN_3) 

#define Check_OpenCloseBit_EN_HIGH      GPIO_SET_H(GPIOB, FL_GPIO_PIN_0)
#define Check_OpenCloseBit_EN_LOW       GPIO_SET_L(GPIOB, FL_GPIO_PIN_0)
#define OpenBitValue                    (FL_GPIO_ReadInputPort(GPIOB)&FL_GPIO_PIN_12)
#define CloseBitValue                   (FL_GPIO_ReadInputPort(GPIOB)&FL_GPIO_PIN_11)
#define LED_Open_HIGH                   GPIO_SET_H(GPIOD, FL_GPIO_PIN_7)
#define LED_Open_LOW                    GPIO_SET_L(GPIOD, FL_GPIO_PIN_7)
#define LED_Close_HIGH                  GPIO_SET_H(GPIOD, FL_GPIO_PIN_8)
#define LED_Close_LOW                   GPIO_SET_L(GPIOD, FL_GPIO_PIN_8)

typedef enum _ValveReasonsDone {
    VALVE_REASON_yuliu1,                // P 回水温度
    VALVE_REASON_yuliu2,                // O 断电开阀
    VALVE_REASON_SELF_CHECK,            // N 自检
    VALVE_REASON_yuliu3,                // M 无线温控器
    VALVE_REASON_CARD_SWIPE,            // L 刷卡
    VALVE_REASON_yuliu4,                // K 系统强制开阀
    VALVE_REASON_SYSTEM_DEADLINE,       // J 系统截止日期
    VALVE_REASON_yuliu5,                // I MBUS接口
    VALVE_REASON_INFRARED_INTERFACE,    // H 红外接口
    VALVE_REASON_NB_MQTT_ONENET_RESERVE // G NB MQTT OneNet
}ValveReasonsDone;

extern void MF_Motor_Init(void);
extern void Valve_SetDegree(uint16_t Set_Degree_Part, ValveReasonsDone DoneId);
#define DegreePartToUint(Number)     ((uint16_t)(Number*10))
#endif
