#include "fm33lc0xx_fl.h"
#include "Define.h"
#include "ADC.h"
#include "AT24CXXDataLoader.h"
#include "PublicLib_No_One.h"
#include "SX1276.h"

#define ADC_VREF        (*((uint16_t *)(0x1FFFFB08)))  //30℃  Vref1.22
#define ADC_TS          (*((uint16_t *)(0x1FFFFa92)))  //30℃ PTAT采样值

static uint32_t GetAVREFSample_POLL(uint32_t * ADCRdresult) {
    uint32_t counter = 0;
    uint32_t EOC_Flag;
    unsigned char State;
    FL_RCC_SetADCPrescaler(FL_RCC_ADC_PSC_DIV8);
    FL_VREF_EnableVREFBuffer(VREF);
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_ALL_CHANNEL);
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_INTERNAL_VREF1P2);
    FL_ADC_ClearFlag_EndOfConversion(ADC);
    FL_ADC_Enable(ADC);
    FL_ADC_EnableSWConversion(ADC);
    do {
        EOC_Flag = FL_ADC_IsActiveFlag_EndOfConversion(ADC);
        counter++;
    } while ((counter != 0x1FFU) && (EOC_Flag == 0U));
    
    if (EOC_Flag == 0x01U) {
        FL_ADC_ClearFlag_EndOfConversion(ADC);
        *ADCRdresult = FL_ADC_ReadConversionData(ADC);

        FL_ADC_Disable(ADC);
        FL_ADC_DisableSequencerChannel(ADC, FL_ADC_INTERNAL_VREF1P2);
        FL_VREF_DisableVREFBuffer(VREF);
        State = 0;
    } else {
        State = 1;
    }
    return State;
}

static uint32_t GetSingleChannelSample_POLL(uint32_t channel, uint32_t * ADCRdresult) {
    uint32_t counter = 0;
    uint32_t EOC_Flag;
    unsigned char State;
    FL_RCC_SetADCPrescaler(FL_RCC_ADC_PSC_DIV8);
    FL_ADC_DisableSequencerChannel(ADC, FL_ADC_ALL_CHANNEL);
    FL_ADC_EnableSequencerChannel(ADC, channel);
    FL_ADC_ClearFlag_EndOfConversion(ADC);
    FL_ADC_Enable(ADC);
    FL_ADC_EnableSWConversion(ADC);
    do {
        EOC_Flag = FL_ADC_IsActiveFlag_EndOfConversion(ADC);
        counter++;
    } while ((counter != 0x1FFU) && (EOC_Flag == 0U));

    if (EOC_Flag == 0x01U) {
        FL_ADC_ClearFlag_EndOfConversion(ADC);
        *ADCRdresult = FL_ADC_ReadConversionData(ADC);
        FL_ADC_Disable(ADC);
        FL_ADC_DisableSequencerChannel(ADC, channel);
        State = 0;
    } else {
        State = 1;
    }
    return State;
}

uint32_t GetSingleChannelVoltage_POLL(uint32_t channel) {
    uint32_t GetAVREFSample = 0, GetChannelVoltage = 0, GetVSample = 0;
    unsigned char  GetAVREFSample_State, GetVSample_State;

    GetAVREFSample_State = GetAVREFSample_POLL(&GetAVREFSample);
    GetVSample_State = GetSingleChannelSample_POLL(channel, &GetVSample);

    if ((GetAVREFSample != 0) && (GetAVREFSample_State == 0) && (GetVSample_State == 0)) {
        GetChannelVoltage = (uint32_t)(((uint64_t)GetVSample * 3000 * (ADC_VREF)) / ((uint64_t)GetAVREFSample * 4095));
    }
    return GetChannelVoltage;
}

unsigned int GetBatVoltage(void) {
    unsigned char i = 0;
    unsigned int temp_test[10] = {0};
    unsigned int testVOL = 0;
    uint64_t GetVSample = 0;
    /*使用 ADC 时 ADCMonitor 功能以及 Vref 需同时开始，同时开启*/
    FL_VREF_Enable(VREF);               //开启VREF1p2
    FL_SVD_EnableADCMonitor(SVD);       //开启ADC电源检测
    FL_ADC_Enable(ADC);                 //开启ADC使能
    IncludeDelayMs(20);
    for (i = 0; i < 10; i++) {
        temp_test[i] = GetSingleChannelVoltage_POLL(FL_ADC_EXTERNAL_CH10);
        GetVSample += temp_test[i];
    }
    GetVSample /= 10;
    testVOL = (unsigned int)GetVSample;

    /*使用 ADC 时 ADCMonitor 功能以及 Vref 需同时开始，同时关闭*/
    FL_VREF_Disable(VREF);               //关闭VREF1p2
    FL_SVD_DisableADCMonitor(SVD);       //关闭ADC电源检测
    FL_ADC_Disable(ADC);                 //关闭ADC使能
    return testVOL;
}
//电池电压测量控制打开 推挽输出低 采样分压电阻的电压
void Battary_GND_ON(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_6;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    FL_GPIO_ResetOutputPin(GPIOA, FL_GPIO_PIN_6);
}
//电池电压测量控制关闭   输入
void Battary_GND_OFF(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_6;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    FL_GPIO_SetOutputPin(GPIOA, FL_GPIO_PIN_6);
}

void Test_BatVoltge(void) {
    MF_ADC_Common_Init();
    MF_ADC_Sampling_Init();
    Battary_GND_ON();
    Now_DEV_Volt = GetBatVoltage() * 3;
    Battary_GND_OFF();
}

unsigned int GetPressVoltage(void) {
    unsigned char i = 0;
    unsigned int temp_test[10] = {0};
    unsigned int testVOL = 0;
    uint64_t GetVSample = 0;
    /*使用 ADC 时 ADCMonitor 功能以及 Vref 需同时开始，同时开启*/
    FL_VREF_Enable(VREF);               //开启VREF1p2
    FL_SVD_EnableADCMonitor(SVD);       //开启ADC电源检测
    FL_ADC_Enable(ADC);                 //开启ADC使能
    IncludeDelayMs(20);
    for (i = 0; i < 10; i++) {
        temp_test[i] = GetSingleChannelVoltage_POLL(FL_ADC_EXTERNAL_CH0);
        GetVSample += temp_test[i];
    }
    GetVSample /= 10;
    testVOL = (unsigned int)GetVSample;

    /*使用 ADC 时 ADCMonitor 功能以及 Vref 需同时开始，同时关闭*/
    FL_VREF_Disable(VREF);               //关闭VREF1p2
    FL_SVD_DisableADCMonitor(SVD);       //关闭ADC电源检测
    FL_ADC_Disable(ADC);                 //关闭ADC使能
    return testVOL * 2;
}

#include "SX1276.h"
extern float Test_PT1000(void);
void TEST_Temperature(void) {
    if (AT24CXX_Manager_NET.Meter_Type == 2) {  // 1：PT1000, 2:压力
        return;
    }
    Now_TemperOrPress = Test_PT1000();
}
void TEST_Press(void) {
    if (AT24CXX_Manager_NET.Meter_Type == 1) {  // 1：PT1000, 2:压力
        return;
    }
    Press_POWER_ON;
    IncludeDelayMs(20);
    // 0--2500  0-2.5MPa
    // 0--3300  0-3.3V
    Now_TemperOrPress = ((GetPressVoltage() * 2500.0) / 3300.0) + (AT24CXX_Manager_NET.MeterPress_Adjust / 10.0);
    Press_POWER_OFF;
}

/**
  * @brief  ADC_Common Initialization function
  * @param  void
  * @retval None
  */
void MF_ADC_Common_Init(void) {
    FL_ADC_CommonInitTypeDef    Common_InitStruct;
    Common_InitStruct.clockSource = FL_RCC_ADC_CLK_SOURCE_RCHF;
    Common_InitStruct.clockPrescaler = FL_RCC_ADC_PSC_DIV8;
    FL_ADC_CommonInit(&Common_InitStruct);
}

/**
  * @brief  ADC_Sampling Initialization function
  * @param  void
  * @retval None
  */
void MF_ADC_Sampling_Init(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct;
    FL_ADC_InitTypeDef    Sampling_InitStruct;

    /* ADC ADC_00   PC9  OUT_3.3V */
    GPIO_InitStruct.pin = FL_GPIO_PIN_9;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    /* ADC ADC_08   PC7  PressV */
    GPIO_InitStruct.pin = FL_GPIO_PIN_7;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    /* ADC ADC_10   PA15  Battary_V */
    GPIO_InitStruct.pin = FL_GPIO_PIN_15;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    Sampling_InitStruct.conversionMode = FL_ADC_CONV_MODE_SINGLE;
    Sampling_InitStruct.autoMode = FL_ADC_SINGLE_CONV_MODE_AUTO;
    Sampling_InitStruct.waitMode = FL_ENABLE;
    Sampling_InitStruct.overrunMode = FL_ENABLE;
    Sampling_InitStruct.scanDirection = FL_ADC_SEQ_SCAN_DIR_FORWARD;
    Sampling_InitStruct.externalTrigConv = FL_ADC_TRIGGER_EDGE_NONE;
    Sampling_InitStruct.triggerSource = FL_ADC_TRGI_PA8;
    Sampling_InitStruct.fastChannelTime = FL_ADC_FAST_CH_SAMPLING_TIME_4_ADCCLK;
    Sampling_InitStruct.lowChannelTime = FL_ADC_SLOW_CH_SAMPLING_TIME_192_ADCCLK;
    Sampling_InitStruct.oversamplingMode = FL_ENABLE;
    Sampling_InitStruct.overSampingMultiplier = FL_ADC_OVERSAMPLING_MUL_8X;
    Sampling_InitStruct.oversamplingShift = FL_ADC_OVERSAMPLING_SHIFT_4B;
    FL_ADC_Init(ADC, &Sampling_InitStruct);
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH0);
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH8);
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH10);
}


