#include "TaskAboutTimer.h"
#include "RTC_SetTime.h"
#include "AT24CXXDataLoader.h"
#include "StrLib.h"
#include "ADC.h"
#include "timers.h"
#include "RF_CMT2300A_TX.h"
#include "event_groups.h"
#include "Define.h"
#include "Task_Valve.h"

EventGroupHandle_t xEventGroup_TimeTask = NULL;
#define xEvent_TestBatV             (1 << 0)
#define xEvent_TestTempe            (1 << 1)
#define xEvent_TestValve            (1 << 2)
#define xEvent_Reboot_433           (1 << 3)
#define xEvent_checkTimeLimit       (1 << 4)
#define xEvent_ForcedOpenValve      (1 << 5)
#define xEvent_ValveAutoTest        (1 << 6)
#define xEvent_NoPowerOfExtOpen     (1 << 7)
#define xEvent_AutoCtrlValveByT2    (1 << 8)
#define xEvent_TestAlarm            (1 << 9)
#define AllxEvent_Timer (xEvent_TestBatV | xEvent_TestTempe | xEvent_TestValve | xEvent_Reboot_433 | xEvent_checkTimeLimit | xEvent_ForcedOpenValve | xEvent_ValveAutoTest | xEvent_NoPowerOfExtOpen | xEvent_AutoCtrlValveByT2 | xEvent_TestAlarm)

TaskHandle_t TimeTaskHand = NULL;
SemaphoreHandle_t TimeTaskSemaphore = NULL;
void startTimeTask(TimerHandle_t xTimer) {
    static uint32_t CallCount = 0;
    CallCount = (CallCount > 3600 ? 0 : CallCount + 1);
    if (TimeTaskSemaphore == NULL) { // 唤醒
        return;
    }
    if (CallCount % (5 * 60) == 0) { // 5 * 60 sec
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableTestBatV)) {
            xEventGroupSetBits(xEventGroup_TimeTask, xEvent_TestBatV); // 设置标志位, 测电压
        }
    }
    if (CallCount % 30 == 0) { // 30 sec
        xEventGroupSetBits(xEventGroup_TimeTask, xEvent_TestTempe);    // 设置标志位, 测温度
    }
    if (CallCount % (1 * 60) == 0) { // 1 * 60 sec
        xEventGroupSetBits(xEventGroup_TimeTask, xEvent_TestValve);    // 设置标志位, 测阀门
    }
    if (CallCount % (30 * 60) == 0) {
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableTimeLimit)) {       // 设置标志位，截止时间 
            xEventGroupSetBits(xEventGroup_TimeTask, xEvent_checkTimeLimit);
        }
    }
    if (CallCount % (30 * 60) == 0) {
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, SuperUserserOpenValve)) { // 设置标志位，强制开阀
            xEventGroupSetBits(xEventGroup_TimeTask, xEvent_ForcedOpenValve);
        }
    }
    if (CallCount % (15 * 24 * 60 * 60) == 0) {
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, ValveAutoTest)) {
            xEventGroupSetBits(xEventGroup_TimeTask, xEvent_ValveAutoTest);
        }
    }
    if (CallCount % (10 * 60) == 0) {
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, AotuCtrlT2)) {
            xEventGroupSetBits(xEventGroup_TimeTask, xEvent_AutoCtrlValveByT2);
        }
    }
    if (CallCount % 1 == 0) {
        xEventGroupSetBits(xEventGroup_TimeTask, xEvent_TestAlarm); // 设置标志位, 检测是否存在报警信息
    }
    // 任意一个事件发生则释放信号量
    EventBits_t uxBits = xEventGroupGetBits(xEventGroup_TimeTask);
    if ((uxBits & AllxEvent_Timer) != 0) {
        xSemaphoreGive(TimeTaskSemaphore); // 释放信号量
    }
}
void NoExtrenPowerAndOpenValve(void) {
    MIN_TASK.CloseTask(NoPwrOpenValve); // 关闭硬件定时器
    static uint16_t TempSave_Degree_Part = 0;
    if (TempSave_Degree_Part == 0) {
        TempSave_Degree_Part = (Get_Module_Data.Now_Degree_Part <= 1000 ? Get_Module_Data.Now_Degree_Part : 1000);
    }
    if (Get_Module_Data.isOpen433 == false) { // 即将阀门全开，暂存当前阀门值
        TempSave_Degree_Part = (Get_Module_Data.Now_Degree_Part <= 1000 ? Get_Module_Data.Now_Degree_Part : 1000);
        Get_Module_Data.CtrlDev_Set_Degree_Part = 1000;
    } else {    // 已恢复供电，准备复位阀门
        Get_Module_Data.CtrlDev_Set_Degree_Part = TempSave_Degree_Part;
    }
    xEventGroupSetBits(xEventGroup_TimeTask, xEvent_NoPowerOfExtOpen); // 触发 NoPowerOfExtOpen 事件
    xSemaphoreGive(TimeTaskSemaphore); // 释放信号量
}
void Test_Exter_Power(TimerHandle_t xTimer) {
    static bool FrontExtervalue = true;
    static int8_t isExternal_PWR = 0;
    if (FrontExtervalue != Read_External_PWR) {
        isExternal_PWR = 0;
        return;
    }
    if (-24 < isExternal_PWR && isExternal_PWR < 24) {
        isExternal_PWR = (Read_External_PWR ? isExternal_PWR + 1 : isExternal_PWR - 1);
        Get_Module_Data.RunTimeFor433 = 0;
    } else {
        isExternal_PWR = (isExternal_PWR >= 24 ? 24 : -24);
        if (Get_Module_Data.RunTimeFor433 < 57600) {// 57600 * 250ms = 4小时
            Get_Module_Data.RunTimeFor433 = (isExternal_PWR == 24 ? Get_Module_Data.RunTimeFor433 + 1 : 0);
        } else {
            // 设置标志位, 重启433
            xEventGroupSetBits(xEventGroup_TimeTask, xEvent_Reboot_433);
        }
    }
    if ((isExternal_PWR == -24) && (Get_Module_Data.isOpen433 == true)) {
        CMT2300A_RF_CLOSE();
        Get_Module_Data.isOpen433 = false;
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, OpenValveForNoPowerOfExtern))
            MIN_TASK.InitSetTimeTask(NoPwrOpenValve, 2, NoExtrenPowerAndOpenValve); // 关闭 433 的同时，准备打开阀门
    } else if ((isExternal_PWR == 24) && (Get_Module_Data.isOpen433 == false)) {
        CMT2300A_RF_OPEN();
        Get_Module_Data.isOpen433 = true;
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, OpenValveForNoPowerOfExtern))
            MIN_TASK.InitSetTimeTask(NoPwrOpenValve, 5, NoExtrenPowerAndOpenValve); // 打开 433 的同时，准备复原阀门
    }
}
void TestValveAutoSub(void) {
    static uint16_t TempSave_Degree_Part = 0;
    TempSave_Degree_Part = Get_Module_Data.Now_Degree_Part;
    if (Get_Module_Data.Now_Degree_Part < 300) {
        Get_Module_Data.CtrlDev_Set_Degree_Part = 1000;
    } else if (Get_Module_Data.Now_Degree_Part > 300) {
        Get_Module_Data.CtrlDev_Set_Degree_Part = 0;
    } else {
        return;
    }
    SetValve TempSetdata = {
        TempSetdata.DoneId = VALVE_REASON_RETURN_WATER_TEMP,
        TempSetdata.MaxDelay_1000Tick = 0,
        TempSetdata.Set_Degree_Part = 0,
    };
    TempSetdata.DoneId = VALVE_REASON_POWER_OFF_OPEN;
    TempSetdata.MaxDelay_1000Tick = 5;
    TempSetdata.Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part;
    ValveCtrlStart(TempSetdata);
    Get_Module_Data.CtrlDev_Set_Degree_Part = (Get_Module_Data.CtrlDev_Set_Degree_Part == 0 ? 1000 : 0);
    TempSetdata.DoneId = VALVE_REASON_POWER_OFF_OPEN;
    TempSetdata.MaxDelay_1000Tick = 5;
    TempSetdata.Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part;
    ValveCtrlStart(TempSetdata);
    Get_Module_Data.CtrlDev_Set_Degree_Part = TempSave_Degree_Part;
    TempSetdata.DoneId = VALVE_REASON_POWER_OFF_OPEN;
    TempSetdata.MaxDelay_1000Tick = 5;
    TempSetdata.Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part;
    ValveCtrlStart(TempSetdata);
    Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
}
#include "NumberBaseLib.h"
void CtrlValveByT2AutoSub(void) {
    if (Get_Module_Data.Now_Temper_T2 < 5 || 75 < Get_Module_Data.Now_Temper_T2) {
        return;
    }
    float T2Part = 0.0;
    float TempPart_i_Min = 0.0;
    float TempPart_i_Max = 0.0;
    for (uint8_t i = 0; i < 4; i++) {
        T2Part = getPartOfSetPointOnRing(Get_Module_Data.Now_Temper_T2, AT24CXX_Manager_NET.T2AutoCtrlBuf[0].T2Min, AT24CXX_Manager_NET.T2AutoCtrlBuf[3].T2Max);
        TempPart_i_Min = getPartOfSetPointOnRing(AT24CXX_Manager_NET.T2AutoCtrlBuf[i].T2Min, AT24CXX_Manager_NET.T2AutoCtrlBuf[0].T2Min, AT24CXX_Manager_NET.T2AutoCtrlBuf[3].T2Max);
        TempPart_i_Max = getPartOfSetPointOnRing(AT24CXX_Manager_NET.T2AutoCtrlBuf[i].T2Max, AT24CXX_Manager_NET.T2AutoCtrlBuf[0].T2Min, AT24CXX_Manager_NET.T2AutoCtrlBuf[3].T2Max);
        if (T2Part == -1) {
            return;
        }
        if (TempPart_i_Min < T2Part && T2Part <= TempPart_i_Max) {
            Get_Module_Data.CtrlDev_Set_Degree_Part = AT24CXX_Manager_NET.T2AutoCtrlBuf[i].T2_Set_Degree_Part;
            if (Get_Module_Data.CtrlDev_Set_Degree_Part != 0xFFFF) {
                SetValve TempSetdata = {
                    .DoneId = VALVE_REASON_SYSTEM_DEADLINE,
                    .MaxDelay_1000Tick = 5,
                    .Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part,
                };
                ValveCtrlStart(TempSetdata);
                Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
            }
            break;
        }
    }
}

/*
    D 温控器  被拆卸报警
    E 温控器  无线失联超2小时
    F 温控器  低电报警
    G 阀门低电报警
    H 刷卡故障
    I  无线故障
*/
void TestSystemAlarm(void) {
    if (Get_Module_Data.Now_DEV_Volt < 250) {
        SetErrerCode(CODE_ERR_VALVE_LOW_BATTERY, true);
    } else {
        SetErrerCode(CODE_ERR_VALVE_LOW_BATTERY, false);
    }
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, Enable433Open)) {
        if (Get_Module_Data.CtrlDev_Now_Volt < 250) {
            SetErrerCode(CODE_ERR_THERMOSTAT_LOW_BATTERY, true);
        } else {
            SetErrerCode(CODE_ERR_THERMOSTAT_LOW_BATTERY, false);
        }
        if (MIN_TASK.Task[testConnectFor433].TimeTask_Falge) {
            MIN_TASK.CloseTask(testConnectFor433);
            SetErrerCode(CODE_ERR_THERMOSTAT_LOST, true);
        } else {
            SetErrerCode(CODE_ERR_THERMOSTAT_LOST, false);
        }
        if (Get_Module_Data.CtrlDev_Drive_Be_Moved) {
            SetErrerCode(CODE_ERR_THERMOSTAT_TAMPER, true);
        } else {
            SetErrerCode(CODE_ERR_THERMOSTAT_TAMPER, false);
        }
    }
}
void TimeTask(void * pvParameters) {
    TimerHandle_t TestExterPWRTimer = xTimerCreate(
        "Test_Exter_PWR",           // 定时器名称
        pdMS_TO_TICKS(250),         // 定时周期 250ms
        pdTRUE,                     // 自动重载
        (void *)0,                  // 定时器 ID
        Test_Exter_Power            // 回调函数
    );
    // 创建软件定时器
    if (TestExterPWRTimer != NULL) {
        xTimerStart(TestExterPWRTimer, 0);  // 启动定时器 测外部供电
    }
    TimerHandle_t TestBatVoltgeTimer = xTimerCreate(
        "Measure_Time",          // 定时器名称
        pdMS_TO_TICKS(1000),     // 定时周期 1s
        pdTRUE,                  // 自动重载
        (void *)0,               // 定时器 ID
        startTimeTask            // 回调函数
    );
    // 创建软件定时器
    if (TestBatVoltgeTimer != NULL) {
        xTimerStart(TestBatVoltgeTimer, 0);  // 启动定时器 测电压
    }
    // 创建硬件定时器
    MIN_TASK.InitSetTimeTask(testConnectFor433, HourToMin_15(2), Test433AlarmTask);
    TimeTaskSemaphore = xSemaphoreCreateBinary(); // 初始化信号量
    // 创建事件组
    xEventGroup_TimeTask = xEventGroupCreate();
    if (xEventGroup_TimeTask == NULL) {
        Reboot.EC20_ReBoot_In_RTC_By_1Second(&Reboot, 10);
    }
    do {
        ////////////////////////////////***测量***////////////////////////////////
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_TestBatV) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_TestBatV);       // 清除事件标志位
            Test_BatVoltge();
        }
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_TestTempe) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_TestTempe);      // 清除事件标志位
            Test_Temperature();
        }
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_TestValve) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_TestValve);      // 清除事件标志位
            if (Get_Module_Data.Now_Degree_Part > 400) {
                Get_Module_Data.TempRunTimeForValve++;
            }
            Test_ValueDegree();
        }
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_Reboot_433) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_Reboot_433);     // 清除事件标志位
            CMT2300A_RF_CLOSE();
            vTaskDelay(10000);
            CMT2300A_RF_OPEN();
        }
        ////////////////////////////////***阀门***////////////////////////////////
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_checkTimeLimit) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_checkTimeLimit);
            uint32_t MaxTime = getTimeNumber_UTCByRTCTime(NEW_NAME(AT24CXX_Manager_NET.LimitTime));
            newString(NowTimeStr, 30);
            sprintf(NowTimeStr.Name._char, "20%02x-%02x-%02x %02x:%02x:%02x", NowYear, NowMonth, NowDay, NowHour, NowMinute, NowSecond);
            uint32_t NowTime = getTimeNumber_UTCByRTCTime(NowTimeStr);
            if (NowTime > MaxTime) {
                Get_Module_Data.CtrlDev_Set_Degree_Part = 0;
            } else {
                if (Get_Module_Data.Now_Degree_Part < 300) {
                    Get_Module_Data.CtrlDev_Set_Degree_Part = 1000;
                }
                Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
            }
            if (Get_Module_Data.CtrlDev_Set_Degree_Part != 0xFFFF) {
                SetValve TempSetdata = {
                    .DoneId = VALVE_REASON_SYSTEM_DEADLINE,
                    .MaxDelay_1000Tick = 5,
                    .Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part,
                };
                ValveCtrlStart(TempSetdata);
                Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
            }
        }
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_ForcedOpenValve) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_ForcedOpenValve);
            Get_Module_Data.CtrlDev_Set_Degree_Part = 1000; //强制开阀
            if (Get_Module_Data.CtrlDev_Set_Degree_Part != 0xFFFF) {
                SetValve TempSetdata = {
                    .DoneId = VALVE_REASON_SYSTEM_DEADLINE,
                    .MaxDelay_1000Tick = 5,
                    .Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part,
                };
                ValveCtrlStart(TempSetdata);
                Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
            }
        }
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_ValveAutoTest) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_ValveAutoTest);
            TestValveAutoSub();
        }
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_NoPowerOfExtOpen) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_NoPowerOfExtOpen);
            if (Get_Module_Data.CtrlDev_Set_Degree_Part != 0xFFFF) {
                SetValve TempSetdata = {
                    .DoneId = VALVE_REASON_POWER_OFF_OPEN,
                    .MaxDelay_1000Tick = 5,
                    .Set_Degree_Part = Get_Module_Data.CtrlDev_Set_Degree_Part,
                };
                ValveCtrlStart(TempSetdata);
                Get_Module_Data.CtrlDev_Set_Degree_Part = 0xFFFF;
            }
        }
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_AutoCtrlValveByT2) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_AutoCtrlValveByT2);
            CtrlValveByT2AutoSub();
        }
        ////////////////////////////////***故障***////////////////////////////////
        if (xEventGroupGetBits(xEventGroup_TimeTask) & xEvent_TestAlarm) {
            xEventGroupClearBits(xEventGroup_TimeTask, xEvent_TestAlarm);
            TestSystemAlarm();
        }
    } while (xSemaphoreTake(TimeTaskSemaphore, portMAX_DELAY) == pdTRUE);   // 等待信号量阻塞任务, 转为运行其他任务
}
void Test433AlarmTask(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // 向 TimeTask 发出信号量, 准备继续运行
    xSemaphoreGiveFromISR(TimeTaskSemaphore, &xHigherPriorityTaskWoken);
    // 恢复 TimeTask 任务
    if (xTaskResumeFromISR(TimeTaskHand) == pdFALSE) {
        // 如果需要切换上下文
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
