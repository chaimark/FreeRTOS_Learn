#include "fm33lc0xx_fl.h"
#include "../Interflow/Define.h"
#include "../Interflow/PublicLib_No_One.h"
#include "Main.h"

#define ADC_VREF        (*((uint16_t *)(0x1FFFFB08)))  //30℃  Vref1.22
#define ADC_TS          (*((uint16_t *)(0x1FFFFa92)))  //30℃ PTAT采样值

const unsigned int InsideT_TABLE[] = {
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
    FL_RCC_SetADCPrescaler(FL_RCC_ADC_PSC_DIV16);
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
        temp_test[i] = GetSingleChannelVoltage_POLL(FL_ADC_EXTERNAL_CH3);
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
void Test_BatVoltge(void) {
    RTC_TASK.InitSetTimeTask(BatVoltge, MinToSec(1), NULL); // 1 分钟检测一次电池电压
    unsigned int testVOL = 0;
    Battary_SET_OUTMODE();
    testVOL = GetBatVoltage();
    Battary_SET_INMODE();
    Now_DEV_Volt = testVOL * 3;
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
    INSIDET_PWR_HIGH;
    // 测量总电阻电压
    Sum_testVOL = GetTempVoltage(FL_ADC_EXTERNAL_CH0);
    // 测量热敏电压
    Heat_testVOL = GetTempVoltage(FL_ADC_EXTERNAL_CH4);

    unsigned int Heat_R_NUM = (10000 * Heat_testVOL) / (Sum_testVOL - Heat_testVOL);

    INSIDET_PWR_LOW;
    // 返回电阻
    return Heat_R_NUM;
}
// void Test_InsideT(void) {
//     RTC_TASK.InitSetTimeTask(InsideT, MinToSec(1), NULL); // 1 分钟检测一次内部温度
//     float Temp_di = 0.0;        // 单位摄氏度的等份数
//     float Temp_Temper = 0.0;    // 当前温度
//     unsigned int AD_Vaule = Get_Temper_value();
//     if (AD_Vaule > 384903) {
//         Now_Temper = 0.0;
//         return;
//     }
//     if (AD_Vaule < 637) {
//         Now_Temper = 0.0;
//         return;
//     }
//     if (InsideT_TABLE[0] == AD_Vaule) {
//         Now_Temper = -40.0;
//         return;
//     }
//     for (int i = 1; i < 140; i++) {
//         if (InsideT_TABLE[i] < AD_Vaule) {
//             Temp_Temper = (float)(i - 40); // 当前温度的整数部分
//             Temp_di = (float)(InsideT_TABLE[i - 1] - InsideT_TABLE[i]);
//             Temp_Temper -= ((AD_Vaule - InsideT_TABLE[i]) / Temp_di);
//             break;
//         }
//     }
//     Now_Temper = Temp_Temper;
// }

