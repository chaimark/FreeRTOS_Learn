#include "adc.h"

#include "FreeRTOS.h"
#include "task.h"

void ADC_Init(void)
{
    FL_GPIO_InitTypeDef    GPIO_InitStruct;  
    FL_ADC_CommonInitTypeDef    CommonInitStruct;
    FL_ADC_InitTypeDef    defaultInitStruct;

 
    GPIO_InitStruct.pin = FL_GPIO_PIN_9;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init( GPIOC, &GPIO_InitStruct );
    
    CommonInitStruct.clockSource = FL_RCC_ADC_CLK_SOURCE_RCHF;
    CommonInitStruct.clockPrescaler = FL_RCC_ADC_PSC_DIV8;
    FL_ADC_CommonInit(&CommonInitStruct );   
    
    defaultInitStruct.conversionMode = FL_ADC_CONV_MODE_SINGLE;
    defaultInitStruct.autoMode = FL_ADC_SINGLE_CONV_MODE_AUTO;
    defaultInitStruct.waitMode = FL_ENABLE;
    defaultInitStruct.overrunMode = FL_ENABLE;
    defaultInitStruct.scanDirection = FL_ADC_SEQ_SCAN_DIR_BACKWARD;
    defaultInitStruct.externalTrigConv = FL_ADC_TRIGGER_EDGE_NONE;
    defaultInitStruct.triggerSource = FL_ADC_TRGI_PA8;
    defaultInitStruct.fastChannelTime = FL_ADC_FAST_CH_SAMPLING_TIME_16_ADCCLK;
    defaultInitStruct.lowChannelTime = FL_ADC_SLOW_CH_SAMPLING_TIME_192_ADCCLK;
    defaultInitStruct.oversamplingMode = FL_ENABLE;
    defaultInitStruct.overSampingMultiplier = FL_ADC_OVERSAMPLING_MUL_16X;
    defaultInitStruct.oversamplingShift = FL_ADC_OVERSAMPLING_SHIFT_4B;

    FL_ADC_Init(ADC,&defaultInitStruct );

}

static uint32_t GetVREF1P2Sample_POLL(void)
{
    uint16_t ADCRdresult;
    uint8_t i=0;
    FL_RCC_SetADCPrescaler(FL_RCC_ADC_PSC_DIV8);
    FL_VREF_EnableVREFBuffer(VREF);//使能VREF BUFFER
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_INTERNAL_VREF1P2);//通道选择VREF
            
    FL_ADC_ClearFlag_EndOfConversion(ADC);//清标志			
    FL_ADC_Enable(ADC);   	 // 启动ADC
    FL_ADC_EnableSWConversion(ADC);  // 开始转换
    // 等待转换完成
    while (FL_ADC_IsActiveFlag_EndOfConversion(ADC) == FL_RESET)
    {
        if(i>=5)
        {
            break;
        }
        i++;
        vTaskDelay(1);
    }
    FL_ADC_ClearFlag_EndOfConversion(ADC);//清标志
    ADCRdresult =FL_ADC_ReadConversionData(ADC);//获取采样值

    FL_ADC_Disable(ADC);    // 关闭ADC
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_INTERNAL_VREF1P2);//通道关闭VREF	
    FL_VREF_DisableVREFBuffer(VREF);//关闭VREF BUFFER			
    // 转换结果 
    return ADCRdresult;
}



static uint32_t GetSingleChannelSample_POLL(uint32_t channel)
{
    uint16_t ADCRdresult;
    uint8_t i=0;
    FL_RCC_SetADCPrescaler(FL_RCC_ADC_PSC_DIV1);
    FL_ADC_EnableSequencerChannel(ADC, channel);//通道选择

    FL_ADC_ClearFlag_EndOfConversion(ADC);//清标志			
    FL_ADC_Enable(ADC);   	 // 启动ADC
    FL_ADC_EnableSWConversion(ADC);  // 开始转换
    // 等待转换完成
    while (FL_ADC_IsActiveFlag_EndOfConversion(ADC) == FL_RESET)
    {
        if(i>=5)
        {
            break;
        }
        i++;
        vTaskDelay(1);
    }
    FL_ADC_ClearFlag_EndOfConversion(ADC);//清标志
    ADCRdresult =FL_ADC_ReadConversionData(ADC);//获取采样值

    FL_ADC_Disable(ADC);    // 关闭ADC
    FL_ADC_DisableSequencerChannel(ADC, channel);//通道	 	 
    // 转换结果 
    return ADCRdresult;
}


uint32_t GetSingleChannelVoltage_POLL(uint32_t channel)
{
    uint32_t Get122VSample,GetChannelVoltage;
    uint64_t GetVSample; 

    Get122VSample = GetVREF1P2Sample_POLL();
    GetVSample =GetSingleChannelSample_POLL(channel);
    GetChannelVoltage =  (GetVSample *3000*(ADC_VREF))/(Get122VSample*4095);
    // 转换结果 
    return GetChannelVoltage;
}

