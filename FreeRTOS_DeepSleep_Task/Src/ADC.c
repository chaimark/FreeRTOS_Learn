#include "fm33lc0xx_fl.h"
#include "Define.h"
#include "ADC.h"
#include "AT24CXXDataLoader.h"
#include "PublicLib_No_One.h"
#include "MotorCtrlDev.h"
#include "PT1000.h"
#include "Task_Valve.h"
#include "GP21.h"

SemaphoreHandle_t ADCTest_RunFlag = NULL;
#define clearADCTestFlag xSemaphoreGive(ADCTest_RunFlag);\
IncludeDelayMs(1);
bool isRunADCTest(uint8_t DelayS_TimeOut) {
    uint8_t NowCount = 0;
    // 获取互斥量，防止读写冲突
    while (!(xSemaphoreTake(ADCTest_RunFlag, 100) == pdTRUE)) {
        NowCount++;
        vTaskDelay(1);
        if (DelayS_TimeOut * 1000 <= NowCount) {
            return false; // 超时退出
        }
    }
    return true;
}
void init_ADCTest_RunFlag(void) {
    static bool isInit_RunFlag = false;
    if (isInit_RunFlag) {
        return;
    }
    ADCTest_RunFlag = xSemaphoreCreateBinary(); // 初始化信号量
    xSemaphoreGive(ADCTest_RunFlag);
}
//////////////////////////////////////////////////////////////
#define ADC_VREF        (*((uint16_t *)(0x1FFFFB08)))  //30℃  Vref1.22
#define ADC_TS          (*((uint16_t *)(0x1FFFFa92)))  //30℃ PTAT采样值

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

static uint32_t GetAVREFSample_POLL(uint32_t * ADCRdresult) {
    uint32_t counter = 0;
    uint32_t EOC_Flag = 0;
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

//电池电压测量控制打开 推挽输出低 采样分压电阻的电压
void Battary_GND_ON(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_1;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    VOL_GND_LOW;
}
//电池电压测量控制关闭   输入
void Battary_GND_OFF(void) {
    FL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = FL_GPIO_PIN_1;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
unsigned int GetTempVoltage(uint32_t Chx_Number) {
    unsigned char i = 0;
    unsigned int temp_test[10] = {0};
    unsigned int testVOL = 0;
    uint64_t GetVSample = 0;
    // 加锁
    if (isRunADCTest(1) == false) {
        return 0; // 超时 1s 退出
    }
    /*使用 ADC 时 ADCMonitor 功能以及 Vref 需同时开始，同时开启*/
    FL_VREF_Enable(VREF);               //开启VREF1p2
    FL_SVD_EnableADCMonitor(SVD);       //开启ADC电源检测
    FL_ADC_Enable(ADC);                 //开启ADC使能
    IncludeDelayMs(50);
    vTaskSuspendAll();  // 阻止OS调度
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
    xTaskResumeAll();	// 恢复OS调度
    clearADCTestFlag; // 释放信号量
    return testVOL;
}

unsigned int Get_Temper_value(uint8_t TemperX) {
    unsigned int Sum_testVOL = 0;
    unsigned int Heat_testVOL = 0;
    GP21_NTC_PWR_HIGH;
    if (TemperX == 1) {
        // 测量总电阻电压
        Sum_testVOL = GetTempVoltage(FL_ADC_EXTERNAL_CH11);
        // 测量热敏电压
        Heat_testVOL = GetTempVoltage(FL_ADC_EXTERNAL_CH9);
    } else {
        // 测量总电阻电压
        Sum_testVOL = GetTempVoltage(FL_ADC_EXTERNAL_CH11);
        // 测量热敏电压
        Heat_testVOL = GetTempVoltage(FL_ADC_EXTERNAL_CH8);
    }
    GP21_NTC_PWR_LOW;
    if (((float)(Heat_testVOL * 1.0) > (float)(Sum_testVOL * 0.9)) || (Sum_testVOL == 0) || (Heat_testVOL == 0)) {
        return 0;
    }
    // 返回电阻
    return (10000 * Heat_testVOL) / (Sum_testVOL - Heat_testVOL);
}
float GetTemper_NTC(uint8_t TemperX) {
    float Temp_di = 0.0;        // 单位摄氏度的等份数
    float Temp_Temper = 0.0;    // 当前温度
    unsigned int AD_Vaule = Get_Temper_value(TemperX);
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
#include "TaskEEprom.h"
unsigned int GetTempDegree(int8_t isSaveR_10K) {
    unsigned int OpenVOL = AT24CXX_Manager_NET.OpenVOL;
    unsigned int CloseVOL = AT24CXX_Manager_NET.CloseVOL;
    if (isSaveR_10K == 1) {
        OpenVOL = GetTempVoltage(FL_ADC_EXTERNAL_CH2); // 开启度电压
        if (CloseVOL == 0xFFFF) {
            CloseVOL = OpenVOL;
        }
        isRunEEprom();
        AT24CXX_Manager_NET.OpenVOL = OpenVOL;
        EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.OpenVOL, sizeof(AT24CXX_Manager_NET.OpenVOL));
        clearEEpromFlag;
    } else if (isSaveR_10K == -1) {
        CloseVOL = GetTempVoltage(FL_ADC_EXTERNAL_CH2); // 关闭度电压
        if (OpenVOL == 0xFFFF) {
            OpenVOL = CloseVOL;
        }
        isRunEEprom();
        AT24CXX_Manager_NET.CloseVOL = CloseVOL;
        EEprom_AT24C0XXData_Write(&AT24CXX_Manager_NET.CloseVOL, sizeof(AT24CXX_Manager_NET.CloseVOL));
        clearEEpromFlag;
    }
    if (OpenVOL == CloseVOL) {
        SetErrerCode(CODE_ERR_VALVE_OPEN_OUT_OF_RANGE, true);
        return 0; // 避免除数为 0
    } else {
        SetErrerCode(CODE_ERR_VALVE_OPEN_OUT_OF_RANGE, false);
    }
    // ADC 测量角度 11 通道
    unsigned int testVOL = GetTempVoltage(FL_ADC_EXTERNAL_CH2);
    float Degree_Part = ((testVOL * 1.0 - CloseVOL * 1.0) / (OpenVOL * 1.0 - CloseVOL * 1.0));
    return floatToint6410Rate(Degree_Part * 100.0); // 返回 0-1000 的整数 0% - 100%
}

void Test_BatVoltge(void) {
    Battary_GND_ON();
    System_RunData.Now_DEV_Volt = GetTempVoltage(FL_ADC_EXTERNAL_CH1) * 3;
    Battary_GND_OFF();
}

void Test_Temperature(void) {
    Check_Temper_Battery_Init();
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableADCTestTemper)) {
        System_RunData.Now_Temper_T1 = GetTemper_NTC(1);
        if (0 < System_RunData.Now_Temper_T1 && System_RunData.Now_Temper_T1 <= 70) {
            SetErrerCode(CODE_ERR_T1_SENSOR_FAULT_ADC, false);
        } else {
            SetErrerCode(CODE_ERR_T1_SENSOR_FAULT_ADC, true);
        }
        System_RunData.Now_Temper_T2 = GetTemper_NTC(2);
        if (0 < System_RunData.Now_Temper_T2 && System_RunData.Now_Temper_T2 <= 70) {
            SetErrerCode(CODE_ERR_T2_SENSOR_FAULT_ADC, false);
        } else {
            SetErrerCode(CODE_ERR_T2_SENSOR_FAULT_ADC, true);
        }
        System_RunData.Now_Temper_T1 = (System_RunData.Now_Temper_T1 < 0 ? 0 : System_RunData.Now_Temper_T1);
        System_RunData.Now_Temper_T2 = (System_RunData.Now_Temper_T2 < 0 ? 0 : System_RunData.Now_Temper_T2);
    } else if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableGP2TestTemper)) {
        Test_PT1000(&System_RunData.Now_Temper_T1, &System_RunData.Now_Temper_T2);
    } else {
        // 设置故障码
        SetErrerCode(CODE_ERR_T1_SENSOR_FAULT_ADC, true);
        SetErrerCode(CODE_ERR_T2_SENSOR_FAULT_ADC, true);
        SetErrerCode(CODE_ERR_T1_SENSOR_FAULT_GP2, true);
        SetErrerCode(CODE_ERR_T2_SENSOR_FAULT_GP2, true);
        System_RunData.Now_Temper_T1 = 0;
        System_RunData.Now_Temper_T2 = 0;
    }
}

void Test_ValueDegree(int8_t isSaveR_10K, bool isSetEquTest) {
    Check_OpenCloseBit_EN_HIGH;
    if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableADCTestValve)) {
        System_RunData.Now_Degree_Part = GetTempDegree(isSaveR_10K);
        if (isSetEquTest) {
            if ((System_RunData.Now_Degree_Part >= ((AT24CXX_Manager_NET.UserSet_DegreePart < DegreePartToUint(1) ? 0 : AT24CXX_Manager_NET.UserSet_DegreePart - DegreePartToUint(1)))) &&
                (System_RunData.Now_Degree_Part <= (AT24CXX_Manager_NET.UserSet_DegreePart + DegreePartToUint(1)))) {
                // 如果当前度数在设置度数的正负 1% 以内, 则直接替换
                System_RunData.Now_Degree_Part = AT24CXX_Manager_NET.UserSet_DegreePart;
            }
        }
    }
    uint8_t MotorStatus = (OpenBitValue == 0 ? true : false);
    MotorStatus = (((MotorStatus << 1) | (CloseBitValue == 0 ? true : false)) & 0x03);
    if (MotorStatus == 3) { // 识别错误
        if (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableADCTestValve)) {
            SetErrerCode(CODE_ERR_MOTOR_LIMIT_FAULT, true);
        }
    } else if (MotorStatus == 2) { // 开
        SetErrerCode(CODE_ERR_MOTOR_LIMIT_FAULT, false);
        System_RunData.Now_Degree_Part = DegreePartToUint(100);
    } else if (MotorStatus == 1) { // 关
        SetErrerCode(CODE_ERR_MOTOR_LIMIT_FAULT, false);
        System_RunData.Now_Degree_Part = 0;
    } else if (MotorStatus == 0) { // 中间
        SetErrerCode(CODE_ERR_MOTOR_LIMIT_FAULT, false);
        if (((System_RunData.Now_Degree_Part == 0) || (System_RunData.Now_Degree_Part >= DegreePartToUint(100))) &&
            (readDataBit(AT24CXX_Manager_NET.ModeCode, EnableADCTestValve) != true)) {
            System_RunData.Now_Degree_Part = DegreePartToUint(50);
        }
    }
    if (isSetEquTest) {
        Check_OpenCloseBit_EN_LOW;
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
    /* PD2 Read 433 Exterior Power */
    GPIO_InitStruct.pin = FL_GPIO_PIN_2;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;// FL_ENABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    // PB1_Battery_GND
    GPIO_InitStruct.pin = FL_GPIO_PIN_1;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    VOL_GND_HIGH;
    /* ADC ADC_02   PD11  ADC_Degree */
    GPIO_InitStruct.pin = FL_GPIO_PIN_11;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    /* ADC ADC_01   PC10  Battery_Test */
    GPIO_InitStruct.pin = FL_GPIO_PIN_10;
    GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    if (isCheckTemper) {
        /* ADC ADC_11   PC6  ADC_NTC_Sum */
        GPIO_InitStruct.pin = FL_GPIO_PIN_6;
        GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
        GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.pull = FL_DISABLE;
        FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        /* ADC ADC_08   PC7  ADC_NTC_Temper */
        GPIO_InitStruct.pin = FL_GPIO_PIN_7;
        GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
        GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.pull = FL_DISABLE;
        GPIO_InitStruct.remapPin = FL_DISABLE;
        FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        /* ADC ADC_09   PC8  ADC_NTC_Temper */
        GPIO_InitStruct.pin = FL_GPIO_PIN_8;
        GPIO_InitStruct.mode = FL_GPIO_MODE_ANALOG;
        GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.pull = FL_DISABLE;
        GPIO_InitStruct.remapPin = FL_DISABLE;
        FL_GPIO_Init(GPIOC, &GPIO_InitStruct);
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
        FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH11);   // PC6
        FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH8);    // PC7
        FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH9);    // PC8
    }
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH1);
    FL_ADC_EnableSequencerChannel(ADC, FL_ADC_EXTERNAL_CH2);
    init_ADCTest_RunFlag();
}


