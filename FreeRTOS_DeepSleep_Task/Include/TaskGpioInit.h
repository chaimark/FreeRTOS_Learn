#ifndef TASKGPIOINIT_H
#define TASKGPIOINIT_H

// enum KeyBitNum {
//     No1_key,
// };

typedef struct _UserKeyData {
    bool isInput;
    uint8_t KeyNum;
}UserKeyData;
extern UserKeyData NowKeyData;

extern void MF_EXTI_Line_Init(void);
extern TaskHandle_t GpioTaskHand;
#define GpioTask_PRIORITY (tskIDLE_PRIORITY + 1)
extern void GpioTask(void * pvParameters);

#endif


