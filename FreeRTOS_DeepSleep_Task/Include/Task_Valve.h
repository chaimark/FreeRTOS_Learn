
#ifndef TASK_VALVE_H
#define TASK_VALVE_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "MotorCtrlDev.h"

extern TaskHandle_t ValveCtrlHand;
#define ValveCtrl_PRIORITY (tskIDLE_PRIORITY + 4)
extern void ValveCtrlTask(void * pvParameters);
////////////////////////////////// 阀门控制的外部 api
typedef struct _SetValve {
    ValveReasonsDone DoneId;
    uint8_t MaxDelay_1000Tick;
    uint16_t Set_Degree_Part;
}SetValve;
extern SetValve SetData_Input;
extern bool ValveCtrlStart(SetValve SetData);
//////////////////////////////////
#endif


