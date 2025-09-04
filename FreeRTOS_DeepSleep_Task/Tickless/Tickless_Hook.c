#include "Tickless_Hook.h" 
#include "FreeRTOS.h"
#include "task.h"
#include "Define.h"
#include "MIN_SetTime.h"

#define   ulGetExternalTime()    FL_LPTIM32_ReadCounter(LPTIM32)

#if( configUSE_TICKLESS_IDLE != 0 )
void sleepConfig(void) {

    /*IO CONFIG*/
    FL_PMU_SleepInitTypeDef    defaultInitStruct;

    defaultInitStruct.deepSleep = FL_PMU_SLEEP_MODE_DEEP;
    defaultInitStruct.LDOLowPowerMode = FL_PMU_LDO_LPM_DISABLE;
    defaultInitStruct.wakeupFrequency = FL_PMU_RCHF_WAKEUP_FREQ_8MHZ;
    defaultInitStruct.wakeupDelay = FL_PMU_WAKEUP_DELAY_2US;
    defaultInitStruct.coreVoltageScaling = FL_DISABLE;

    FL_PMU_Sleep_Init(PMU, &defaultInitStruct);

    FL_RCC_RCMF_Disable();               //关闭RCMF
    FL_RMU_PDR_Enable(RMU);  			 //打开PDR
    FL_RMU_BORPowerDown_Disable(RMU);  	 //关闭BOR 2uA

    /*使用ADC时ADCMonitor功能以及Vref需同时开始，同时关闭*/
    FL_VREF_Disable(VREF);               //关闭VREF1p2
    FL_SVD_DisableADCMonitor(SVD);       //关闭ADC电源检测
    FL_ADC_Disable(ADC);                 //关闭ADC使能

    FL_PMU_SetLowPowerMode(PMU, FL_PMU_POWER_MODE_SLEEP_OR_DEEPSLEEP);
    __WFI();
}
void prvStopTickInterruptTimer(void) {
    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_ClearPendingIRQ(SysTick_IRQn);
    SysTick->CTRL = 0;
}

void disable_interrupts(void) {
    __asm volatile ("cpsid i");
    __asm volatile ("dsb");
    __asm volatile ("isb");
}

void prvStartTickInterruptTimer(void) {
    SysTick_Config(configCPU_CLOCK_HZ / configTICK_RATE_HZ);
    NVIC_EnableIRQ(SysTick_IRQn);
}

void enable_interrupts(void) {
    __asm volatile ("cpsie i");
}

void prvSleep(void) {
    LPTIM_Setup();
    sleepConfig();
    LPTIM_Stop();
}

void vSetWakeTimeInterrupt(TickType_t xExpectedIdleTime) {
    FL_LPTIM32_WriteCompareCH1(LPTIM32, xExpectedIdleTime + ulGetExternalTime());
}

void vApplicationSleep(TickType_t xExpectedIdleTime) {
    unsigned long ulLowPowerTimeBeforeSleep, ulLowPowerTimeAfterSleep;
    eSleepModeStatus eSleepStatus;
     // 进入低功耗模式前的处理
    if (configPRE_SLEEP_PROCESSING(NULL) == false) {
        // 暂时不进入低功耗
        return;
    }
    /* Read the current time from a time source that will remain operational
    while the microcontroller is in a low power state. */
    ulLowPowerTimeBeforeSleep = ulGetExternalTime();

    /* Stop the timer that is generating the tick interrupt. */
    prvStopTickInterruptTimer();

    /* Enter a critical section that will not effect interrupts bringing the MCU
    out of sleep mode. */
    disable_interrupts(); // 屏蔽所有中断

    /* Ensure it is still ok to enter the sleep mode. */
    eSleepStatus = eTaskConfirmSleepModeStatus();

    if (eSleepStatus == eAbortSleep) {
      /* A task has been moved out of the Blocked state since this macro was
      executed, or a context siwth is being held pending.  Do not enter a
      sleep state.  Restart the tick and exit the critical section. */
        prvStartTickInterruptTimer();
        enable_interrupts();
    } else {
        if (eSleepStatus == eNoTasksWaitingTimeout) {
            /* It is not necessary to configure an interrupt to bring the
            microcontroller out of its low power state at a fixed time in the
            future. */
            prvSleep();
            /* Exit the critical section - it might be possible to do this immediately
            after the prvSleep() calls. */
            enable_interrupts();
        } else {
            /* Configure an interrupt to bring the microcontroller out of its low
            power state at the time the kernel next needs to execute.  The
            interrupt must be generated from a source that remains operational
            when the microcontroller is in a low power state. */
            vSetWakeTimeInterrupt(xExpectedIdleTime - 1);
            /* Enter the low power state. */
#ifdef OPEN_LOWPWER_DEBUG
            /* 需要模拟唤醒时,启动低功耗调试任务，即可退出等待
            需要模拟睡眠时，关闭低功耗调试任务，即可开启等待*/
            while (!RTC_TASK.Task[LowPwerDebug].isTaskStart);
#else
            prvSleep();
#endif
            /* Exit the critical section - it might be possible to do this immediately
            after the prvSleep() calls. */
            enable_interrupts();
            /* Determine how long the microcontroller was actually in a low power
            state for, which will be less than xExpectedIdleTime if the
            microcontroller was brought out of low power mode by an interrupt
            other than that configured by the vSetWakeTimeInterrupt() call.
            Note that the scheduler is suspended before
            portSUPPRESS_TICKS_AND_SLEEP() is called, and resumed when
            portSUPPRESS_TICKS_AND_SLEEP() returns.  Therefore no other tasks will
            execute until this function completes. */
            ulLowPowerTimeAfterSleep = ulGetExternalTime();
            /* Correct the kernels tick count to account for the time the
            microcontroller spent in its low power state. */
            vTaskStepTick((ulLowPowerTimeAfterSleep - ulLowPowerTimeBeforeSleep));
        }
        /* Restart the timer that is generating the tick interrupt. */
        prvStartTickInterruptTimer();
        configPOST_SLEEP_PROCESSING(NULL);  // 退出低功耗模式后的处理
    }
}
#endif
