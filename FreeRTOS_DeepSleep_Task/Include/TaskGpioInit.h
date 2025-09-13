#ifndef TASKGPIOINIT_H
#define TASKGPIOINIT_H

// enum KeyBitNum {
//     No1_key,
// };
#include "Define.h"
#define ReadKeyValue1                   ((FL_GPIO_ReadInputPort(GPIOC)&FL_GPIO_PIN_4))
#define KEY1_HIGH                        GPIO_SET_H(GPIOC,FL_GPIO_PIN_4)
#define KEY1_LOW                         GPIO_SET_L(GPIOC,FL_GPIO_PIN_4)
#define ReadKeyValue2                   ((FL_GPIO_ReadInputPort(GPIOC)&FL_GPIO_PIN_11))
#define KEY2_HIGH                        GPIO_SET_H(GPIOC,FL_GPIO_PIN_11)
#define KEY2_LOW                         GPIO_SET_L(GPIOC,FL_GPIO_PIN_11)


typedef struct _UserKeyData {
    bool isEnableKey;
    bool isInput;
    uint8_t KeyNum;
}UserKeyData;
extern UserKeyData NowKeyData;

extern void MF_EXTI_Line_Init(void);
extern TaskHandle_t GpioTaskHand;
#define GpioTask_PRIORITY (tskIDLE_PRIORITY + 4)
extern void GpioTask(void * pvParameters);
#endif


