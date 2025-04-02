#include "taskgpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern SemaphoreHandle_t   gpioSemaphore;

void GPIOTask(void *pvParameters)
{
    pvParameters = pvParameters;    
    
    while (1)
    {   
     
        if (xSemaphoreTake(gpioSemaphore,portMAX_DELAY) == pdPASS)
        {
            FL_GPIO_ToggleOutputPin(GPIOC, FL_GPIO_PIN_0);  
        }

    }
}
void GPIO_IRQCallBack(void)
{
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  
    xSemaphoreGiveFromISR(gpioSemaphore,&xHigherPriorityTaskWoken );
}
