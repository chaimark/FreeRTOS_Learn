#include "fm33lc0xx_fl.h"
#include "Define.h"
#include "ADC.h"
#include "AT24CXXDataLoader.h"
#include "PublicLib_No_One.h"

#define ADC_VREF        (*((uint16_t *)(0x1FFFFB08)))  //30℃  Vref1.22
#define ADC_TS          (*((uint16_t *)(0x1FFFFa92)))  //30℃ PTAT采样值
#warning "RT 表待修改";
const unsigned int Temper_NTC_TABLE[] = {
    384903,	358864,	334827,	312615,	292068,	273045,	255416,	239065,	223888,	209787, // -40 ~ -31
    196679,	184484,	173130,	162552,	152692,	143494,	134910,	126894,	119405,	112403, // -30 ~ -21
    105854,	99726,	93989,	88615,	83579,	78859,	74431,	70277,	66379,	62717,  // -20 ~ -11
    59278,	56047,	53009,	50152,	47465,	44936,	42556,	40314,	38203,	36213,  // -10 ~ -1
    34338,	32569,	30902,	29328,	27843,	26441,	25118,	23867,	22686,	21569,  // 0 ~ 9
    20514,	19515,	18571,	17678,	16832,	16031,	15273,	14555,	13874,	13229,  // 10 ~ 19
    12617,	12038,	11488,	10966,	10470,	10000,	9554,	9129,	8727,	8344,   // 20 ~ 29
    7980,	7634,	7304,	6991,	6693,	6410,	6140,	5883,	5638,	5405,   // 30 ~ 39
    5182,	4970,	4768,	4575,	4392,	4216,	4049,	3889,	3736,	3591,   // 40 ~ 49
    3451,	3318,	3191,	3070,	2954,	2843,	2736,	2634,	2537,	2444,   // 50 ~ 59
    2354,	2268,	2186,	2108,	2032,	1960,	1890,	1824,	1760,	1699,   // 60 ~ 69
    1640,	1583,	1529,	1477,	1427,	1379,	1332,	1288,	1245,	1204,   // 70 ~ 79
    1164,	1126,	1090,	1054,	1020,	988,	956,	926,	897,	869,    // 80 ~ 89
    842,	815,	790,	766,	742,	720,	698,	677,	657,	637     // 90 ~ 99
};
#warning "RD 表待修改";
const unsigned int Degree_RC_TABLE[] = {
    384903,	358864,	334827,	312615,	292068,	273045,	255416,	239065,	223888,	209787, // -40 ~ -31
    196679,	184484,	173130,	162552,	152692,	143494,	134910,	126894,	119405,	112403, // -30 ~ -21
    105854,	99726,	93989,	88615,	83579,	78859,	74431,	70277,	66379,	62717,  // -20 ~ -11
    59278,	56047,	53009,	50152,	47465,	44936,	42556,	40314,	38203,	36213,  // -10 ~ -1
    34338,	32569,	30902,	29328,	27843,	26441,	25118,	23867,	22686,	21569,  // 0 ~ 9
    20514,	19515,	18571,	17678,	16832,	16031,	15273,	14555,	13874,	13229,  // 10 ~ 19
    12617,	12038,	11488,	10966,	10470,	10000,	9554,	9129,	8727,	8344,   // 20 ~ 29
    7980,	7634,	7304,	6991,	6693,	6410,	6140,	5883,	5638,	5405,   // 30 ~ 39
    5182,	4970,	4768,	4575,	4392,	4216,	4049,	3889,	3736,	3591,   // 40 ~ 49
    3451,	3318,	3191,	3070,	2954,	2843,	2736,	2634,	2537,	2444,   // 50 ~ 59
    2354,	2268,	2186,	2108,	2032,	1960,	1890,	1824,	1760,	1699,   // 60 ~ 69
    1640,	1583,	1529,	1477,	1427,	1379,	1332,	1288,	1245,	1204,   // 70 ~ 79
    1164,	1126,	1090,	1054,	1020,	988,	956,	926,	897,	869,    // 80 ~ 89
    842,	815,	790,	766,	742,	720,	698,	677,	657,	637     // 90 ~ 99
};

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
    GPIO_SET_L(GPIOA, FL_GPIO_PIN_6);
}
//电池电压测量控制关闭   输入
void Battary_GND_OFF(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_6;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SET_H(GPIOA, FL_GPIO_PIN_6);
}

void Test_BatVoltge(void) {
    MF_ADC_Common_Init();
    MF_ADC_Sampling_Init(false);
    Battary_GND_ON();
    Get_Module_Data.Now_DEV_Volt = GetBatVoltage() * 3;
    Battary_GND_OFF();
}

unsigned int GetTempVoltage(uint32_t Chx_Number) {
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
        temp_test[i] = GetSingleChannelVoltage_POLL(Chx_Number);
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
unsigned int Get_Temper_value(void) {
    unsigned int Sum_testVOL = 0;
    unsigned int Heat_testVOL = 0;
    Temper_NTC_PWR_HIGH;
    // 测量总电阻电压
    Sum_testVOL = GetTempVoltage(FL_ADC_EXTERNAL_CH0);
    // 测量热敏电压
    Heat_testVOL = GetTempVoltage(FL_ADC_EXTERNAL_CH4);

    unsigned int Heat_R_NUM = (10000 * Heat_testVOL) / (Sum_testVOL - Heat_testVOL);

    Temper_NTC_PWR_LOW;
    // 返回电阻
    return Heat_R_NUM;
}
float GetTemper_NTC(void) {
    float Temp_di = 0.0;        // 单位摄氏度的等份数
    float Temp_Temper = 0.0;    // 当前温度
    unsigned int AD_Vaule = Get_Temper_value();
    if (AD_Vaule > 384903) {
        return 0.0;
    }
    if (AD_Vaule < 637) {
        return 0.0;
    }
    if (Temper_NTC_TABLE[0] == AD_Vaule) {
        return -40.0;
    }
    for (int i = 1; i < 140; i++) {
        if (Temper_NTC_TABLE[i] < AD_Vaule) {
            Temp_Temper = (float)(i - 40); // 当前温度的整数部分
            Temp_di = (float)(Temper_NTC_TABLE[i - 1] - Temper_NTC_TABLE[i]);
            Temp_Temper -= ((AD_Vaule - Temper_NTC_TABLE[i]) / Temp_di);
            break;
        }
    }
    return Temp_Temper;
}
unsigned int GetTempDegree(uint32_t Chx_Number) {
    // unsigned char i = 0;
    // unsigned int temp_test[10] = {0};
    unsigned int testVOL = 0;
#warning "测角度电阻";
    return testVOL;
}
#include "PT1000.h"
void Test_Temperature(void) {
    Check_Temper_Battery_Init();
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableADCTestTemper)) {
#warning "ADC 测量通道需要调整";
        Get_Module_Data.Now_Temper_T1 = GetTemper_NTC();
        if (0 <= Get_Module_Data.Now_Temper_T1 && Get_Module_Data.Now_Temper_T1 <= 70) {
            SetErrerCode(CODE_ERR_T1_SENSOR_FAULT_ADC, false);
        } else {
            SetErrerCode(CODE_ERR_T1_SENSOR_FAULT_ADC, true);
        }
        Get_Module_Data.Now_Temper_T2 = GetTemper_NTC();
        if (0 <= Get_Module_Data.Now_Temper_T2 && Get_Module_Data.Now_Temper_T2 <= 70) {
            SetErrerCode(CODE_ERR_T2_SENSOR_FAULT_ADC, false);
        } else {
            SetErrerCode(CODE_ERR_T2_SENSOR_FAULT_ADC, true);
        }
        SetErrerCode(CODE_ERR_T2_SENSOR_FAULT_ADC, true);
    } else if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableGP2TestTemper)) {
        Test_PT1000(&Get_Module_Data.Now_Temper_T1, &Get_Module_Data.Now_Temper_T2);
    } else {
        // 设置故障码
        SetErrerCode(CODE_ERR_T1_SENSOR_FAULT_ADC, true);
        SetErrerCode(CODE_ERR_T2_SENSOR_FAULT_ADC, true);
        SetErrerCode(CODE_ERR_T1_SENSOR_FAULT_GP2, true);
        SetErrerCode(CODE_ERR_T2_SENSOR_FAULT_GP2, true);
        Get_Module_Data.Now_Temper_T1 = 0;
        Get_Module_Data.Now_Temper_T2 = 0;
    }
}

uint16_t GetMotorDegree(void) {
    unsigned int Degree_testVOL = GetTempDegree(FL_ADC_EXTERNAL_CH11);
#warning "查 Degree_RC_TABLE 对应的值是阀门开度 百分比, 下列算法需要调整";
    if (Degree_testVOL > 384903) {
        SetErrerCode(CODE_ERR_VALVE_OPEN_FAULT, true);
        return 0;
    }
    if (Degree_testVOL < 637) {
        SetErrerCode(CODE_ERR_VALVE_OPEN_FAULT, true);
        return 0;
    }
    SetErrerCode(CODE_ERR_VALVE_OPEN_FAULT, false);
    if (Degree_RC_TABLE[0] == Degree_testVOL) {
        return 0;
    }
    float Temp_Degree = 0.0;
    float Temp_di = 0.0;        // 单位角度的等份数
    for (int i = 1; i < 140; i++) {
        if (Degree_RC_TABLE[i] < Degree_testVOL) {
            Temp_Degree = (float)(i - 40); // 当前角度的整数部分
            Temp_di = (float)(Degree_RC_TABLE[i - 1] - Degree_RC_TABLE[i]);
            Temp_Degree -= ((Degree_testVOL - Degree_RC_TABLE[i]) / Temp_di);
            break;
        }
    }
#warning "Temp_Degree 与 总角度的占比关系";
    if (Temp_Degree > 1000) {
        SetErrerCode(CODE_ERR_VALVE_OPEN_OUT_OF_RANGE, true);
    }
    return (uint8_t)Temp_Degree;
}

void Test_ValueDegree(void) {
    Motor_PWR_HIGH;
    // ADC 测量角度 11 通道
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableADCTestValve)) {
        Get_Module_Data.Now_Degree_Part = GetMotorDegree();
        if (Get_Module_Data.Now_Degree_Part > 950) {
            Get_Module_Data.Now_Degree_Part = 1000;
        }
        if (Get_Module_Data.Now_Degree_Part < 40) {
            Get_Module_Data.Now_Degree_Part = 0;
        }
    }
    uint8_t MotorStatus = (((CloseBitValue << 1) & OpenBitValue) & 0x03);
    if (MotorStatus == 0) { // 识别错误
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableADCTestValve)) {
            SetErrerCode(CODE_ERR_MOTOR_LIMIT_FAULT, true);
        }
    } else if (MotorStatus == 1) { // 关
        SetErrerCode(CODE_ERR_MOTOR_LIMIT_FAULT, false);
        Get_Module_Data.Now_Degree_Part = 0;
    } else if (MotorStatus == 2) { // 开
        SetErrerCode(CODE_ERR_MOTOR_LIMIT_FAULT, false);
        Get_Module_Data.Now_Degree_Part = 1000;
    } else if (MotorStatus == 3) { // 中间
        SetErrerCode(CODE_ERR_MOTOR_LIMIT_FAULT, false);
    }
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
void MF_ADC_Sampling_Init(bool isCheckTemper) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct;
    FL_ADC_InitTypeDef    Sampling_InitStruct;
    // PC5_Battery_GND
    GPIO_InitStruct.pin = FL_GPIO_PIN_5;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    VOL_GND_HIGH;
    if (isCheckTemper) {
        // PC0_Temper_NTC_PWR
        GPIO_InitStruct.pin = FL_GPIO_PIN_0;
        GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
        GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.pull = FL_DISABLE;
        FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        Temper_NTC_PWR_LOW;
    }

    /* ADC ADC_11   PC6  ADC_Degree */
    GPIO_InitStruct.pin = FL_GPIO_PIN_6;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    /* ADC ADC_08   PC7  Motor_EN */
    GPIO_InitStruct.pin = FL_GPIO_PIN_7;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    Motor_PWR_LOW;
    if (isCheckTemper) {
        /* ADC ADC_00   PC9  Temper_RMAX */
        GPIO_InitStruct.pin = FL_GPIO_PIN_9;
        GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
        GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.pull = FL_DISABLE;
        GPIO_InitStruct.remapPin = FL_DISABLE;
        FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#warning "缺少 ADC 采样通道";
/* ADC ADC_XX   PXX  Temper_Input */
/* ADC ADC_XX   PXX  Temper_Output */
    }

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
    if (isCheckTemper) {
        FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH0);
        // FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CHXX);
        // FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CHXX);
    }
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH8);
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH11);
}


