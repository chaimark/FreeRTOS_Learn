#include "MotorCtrlDev.h"
#include "ADC.h"
#include "Task_Valve.h"
#include "Display.h"

void MF_Motor_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    //PB2_BL113_IN1   PB3_BL113_IN2   PB14_PWR_Ctrl
    GPIO_InitStruct.pin = FL_GPIO_PIN_2 | FL_GPIO_PIN_3 | FL_GPIO_PIN_14;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_SET_L(GPIOB, FL_GPIO_PIN_2 | FL_GPIO_PIN_3 | FL_GPIO_PIN_14);

    /* 限位开关 */
    // PB12_Close_Max     // PB13_Open_Max
    GPIO_InitStruct.pin = FL_GPIO_PIN_12 | FL_GPIO_PIN_13;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //PC12_Open_LED
    GPIO_InitStruct.pin = FL_GPIO_PIN_12;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_SET_L(GPIOC, FL_GPIO_PIN_12);
    //PD11_Close_LED
    GPIO_InitStruct.pin = FL_GPIO_PIN_11;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    GPIO_SET_L(GPIOD, FL_GPIO_PIN_11);
}
void Valve_SetDegree(uint16_t Set_Degree_Part, ValveReasonsDone DoneId) {
    Get_Module_Data.FronValveDoneCode = DoneId; // 保存操作的 id
    // 先刹车 1 1
    BL113_IN1_HIGH;
    BL113_IN2_HIGH;
    // 设置转动方向
    RTC_TASK.CloseTask(HomePageRefresh);
    if (Set_Degree_Part < Get_Module_Data.Now_Degree_Part) { // 正转
        BL113_IN1_LOW;
        BL113_IN2_HIGH;
        ShowOpenPage();
    } else {
        BL113_IN1_HIGH;
        BL113_IN2_LOW;
        ShowClosePage();
    }
    float Part_Di = 0;
    MIN_TASK.InitSetTimeTask(ValveDoneErrMaxTime, 2, NULL);
    do {
        Test_ValueDegree();
        if (Get_Module_Data.Now_Degree_Part > Set_Degree_Part) {
            Part_Di = Get_Module_Data.Now_Degree_Part - Set_Degree_Part;
        } else {
            Part_Di = Set_Degree_Part - Get_Module_Data.Now_Degree_Part;
        }
        vTaskDelay(500);
    } while (Part_Di > 1 && (MIN_TASK.Task[ValveDoneErrMaxTime].TimeTask_Falge == false));
    MIN_TASK.CloseTask(ValveDoneErrMaxTime);
    // 小于 1% 刹车
    BL113_IN1_HIGH;
    BL113_IN2_HIGH;
    RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
}



