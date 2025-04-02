#include "taskgpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>
extern SemaphoreHandle_t   gpioSemaphore;
extern QueueHandle_t   adcSemaphore; 



#ifdef CPU_UTILIZATION
char RunTimeInfo[400];
uint32_t RunTimeTicks;

void BaseTime(void)
{
    FL_BSTIM32_InitTypeDef    defaultInitStruct;
    FL_NVIC_ConfigTypeDef    InterruptConfigStruct;
    defaultInitStruct.prescaler = 8-1;
    defaultInitStruct.autoReload = 50-1;
    defaultInitStruct.autoReloadState = FL_ENABLE;
    defaultInitStruct.clockSource = FL_RCC_BSTIM32_CLK_SOURCE_APB2CLK;
    FL_BSTIM32_Init(BSTIM32,&defaultInitStruct );

    InterruptConfigStruct.preemptPriority = 0x00;
    FL_NVIC_Init(&InterruptConfigStruct,BSTIM_IRQn );

    FL_BSTIM32_EnableIT_Update(BSTIM32 );
    FL_BSTIM32_Enable(BSTIM32);    
}   

void BSTIM_IRQHandler()
{
    if(FL_BSTIM32_IsEnabledIT_Update(BSTIM32) && FL_BSTIM32_IsActiveFlag_Update(BSTIM32))
    {
        FL_BSTIM32_ClearFlag_Update(BSTIM32);
        RunTimeTicks++;
        
    }
}

 #endif
 
 
void GPIOTask(void *pvParameters)
{
    pvParameters = pvParameters;    
    
    while (1)
    {   
     
        if (xSemaphoreTake(gpioSemaphore,portMAX_DELAY) == pdPASS)
        {
            #ifdef CPU_UTILIZATION
                memset(RunTimeInfo,0,400);
                vTaskGetRunTimeStats(RunTimeInfo);
                printf("任务名\t\t运行时间\t运行所占百分比\r\n");
                printf("%s\r\n",RunTimeInfo);            
            #else
                printf("KEY is pressed\r\n");
            #endif
        }
        vTaskDelay(1000);
    }
}
void GPIO_IRQCallBack(void)
{
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  
    xSemaphoreGiveFromISR(gpioSemaphore,&xHigherPriorityTaskWoken );
    xSemaphoreGiveFromISR(adcSemaphore,&xHigherPriorityTaskWoken );
}
