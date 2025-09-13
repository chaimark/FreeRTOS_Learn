#include "MotorCtrlDev.h"
#include "ADC.h"
#include "Task_Valve.h"
#include "Display.h"
#include "timers.h"
#include "AT24CXXDataLoader.h"
#include "NumberBaseLib.h"

void MF_Motor_Init(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    // PB2_BL113_IN1   PB3_BL113_IN2   PB0_PWR_Ctrl
    GPIO_InitStruct.pin = FL_GPIO_PIN_2 | FL_GPIO_PIN_3 | FL_GPIO_PIN_0;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_SET_L(GPIOB, FL_GPIO_PIN_2 | FL_GPIO_PIN_3 | FL_GPIO_PIN_0);
    Check_OpenCloseBit_EN_LOW;

    /* 限位开关 */
    // PB11_Close_Max     // PB12_Open_Max
    GPIO_InitStruct.pin = FL_GPIO_PIN_11 | FL_GPIO_PIN_12;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_ENABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
uint16_t CountValveDegree_Now(uint16_t NowCount, uint16_t MaxCount) {
    if (MaxCount == 0) {
        return 0; // 防止除数为 0
    }
    return DegreePartToUint(floatToint6410Rate((NowCount * 10.0) / (MaxCount * 1.0)));
}

#include "TaskAboutTimer.h"
#define PrecisionNum 0.6
bool isCtrlOver(uint16_t Part_Di, uint16_t Set_Degree_Part) {
    if (Set_Degree_Part > System_RunData.Now_Degree_Part) { // 正转
        setLED_Or_CtrlLinght(false, true, true, 3);
        ShowOpenPage();
        BL113_IN1_HIGH;
        BL113_IN2_LOW;
    } else {
        setLED_Or_CtrlLinght(true, false, true, 3);
        ShowClosePage();
        BL113_IN1_LOW;
        BL113_IN2_HIGH;
    }
    if ((Set_Degree_Part == 0) || (Set_Degree_Part == 100)) {
        uint8_t MotorStatus = (OpenBitValue == 0 ? true : false);
        MotorStatus = (((MotorStatus << 1) | (CloseBitValue == 0 ? true : false)) & 0x03);
        if (MotorStatus == 0) {
            return false; // 中间
        }
    }
    if (Part_Di <= DegreePartToUint(PrecisionNum)) {
        // 小于 1% 刹车
        BL113_IN1_HIGH;
        BL113_IN2_HIGH;
        return true;
    }
    return false;
}
void Valve_SetDegree(uint16_t Set_Degree_Part, ValveReasonsDone DoneId) {
    static uint16_t Max_Time_Count = 12000; // 单次从 0 到 100% 循环的最大次数 每次 10ms
    int8_t isSaveCount = 0; // 是否保存计数
    if (DoneId == VALVE_REASON_SELF_CHECK) {
        if ((System_RunData.Now_Degree_Part == 0) && (Set_Degree_Part == DegreePartToUint(100))) {
            isSaveCount = 1; // 如果是自检任务, 且从 0 到 100 则需要保存计数
        }
        if ((System_RunData.Now_Degree_Part == DegreePartToUint(100)) && (Set_Degree_Part == 0)) {
            isSaveCount = -1; // 如果是自检任务, 且从 100 到 0 则需要保存计数
        }
    }
    // 如果当前度数在设置度数的正负 PrecisionNum 以内, 则不进行操作
    if ((System_RunData.Now_Degree_Part >= ((Set_Degree_Part < DegreePartToUint(PrecisionNum) ? 0 : Set_Degree_Part - DegreePartToUint(PrecisionNum)))) &&
        (System_RunData.Now_Degree_Part <= (Set_Degree_Part + DegreePartToUint(PrecisionNum)))) {
        return; // 如果当前度数在设置度数的正负 PrecisionNum 以内, 则不进行操作
    }
    if (Set_Degree_Part < DegreePartToUint(4)) {
        Set_Degree_Part = 0;
    }
    if (Set_Degree_Part > DegreePartToUint(95)) {
        Set_Degree_Part = DegreePartToUint(100); // 如果当前度数大于 95%, 则设置为 100%
    }
    System_RunData.FronValveDoneCode = DoneId;                     // 保存操作的 id
    AT24CXX_Manager_NET.UserSet_DegreePart = Set_Degree_Part;       // 保存操作的角度
    // 先刹车 1 1
    BL113_IN1_HIGH;
    BL113_IN2_HIGH;
    RTC_TASK.CloseTask(HomePageRefresh);
    RTC_TASK.InitSetTimeTask(ValveDoneErrMaxTime, MinToSec(2), NULL);
    uint16_t Temp_Max_Time_Count = 0;
    int16_t Part_Di = 9999;
    do {
        Temp_Max_Time_Count++;
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableADCTestValve) == false) {
            System_RunData.Now_Degree_Part = CountValveDegree_Now(Temp_Max_Time_Count, Max_Time_Count);
        }
        if (isCtrlOver(Part_Di, Set_Degree_Part)) {
            break; // 如果 Part_Di 大于 PrecisionNum 则跳出循环
        }
        Test_ValueDegree(0, false); // 不启用 ADC 时, 除非限位，不会更新 Now_Degree_Part
        Part_Di = Set_Degree_Part - System_RunData.Now_Degree_Part;
        Part_Di = ((Part_Di < 0) ? 0 - Part_Di : Part_Di); // 取绝对值
        if (((System_RunData.Now_Degree_Part == 0) || System_RunData.Now_Degree_Part == DegreePartToUint(100)) &&
            ((Set_Degree_Part != 0) && (Set_Degree_Part != DegreePartToUint(100))) &&
            (Temp_Max_Time_Count > 40)) {
            break;
        }
    } while (RTC_TASK.Task[ValveDoneErrMaxTime].TimeTask_Falge == false);
    // 小于 1% 刹车
    BL113_IN1_HIGH;
    BL113_IN2_HIGH;
    setLED_Or_CtrlLinght(false, false, true, 1);
    RTC_TASK.CloseTask(ValveDoneErrMaxTime);
    Test_ValueDegree(isSaveCount, true);
    if ((DoneId == VALVE_REASON_SELF_CHECK) && (isSaveCount != 0)) {    // 如果控制阀门的是自检任务, 需要记录并保存运行时间
        Max_Time_Count = Temp_Max_Time_Count;
        isSaveCount = 0;
    }
    RTC_TASK.InitSetTimeTask(HomePageRefresh, 0, ShowHomePage);
}
