#ifndef __DEFINE_H
#define __DEFINE_H
// #include "LPUart.h"
// #include "Uart1.h"

#include "fm33lc0xx_fl_rcc.h"
// #include "fm33lc0xx_fl_lptim.h"
#include "fm33lc0xx_fl_lpuart.h"
#include "fm33lc0xx_fl_gpio.h"
#include "fm33lc0xx_fl_uart.h"
#include "fm33lc0xx_fl_vref.h"
#include "fm33lc0xx_fl_iwdt.h"
#include "fm33lc0xx_fl_wwdt.h"
#include "fm33lc0xx_fl_pmu.h"
#include "fm33lc0xx_fl_flash.h"
#include "fm33lc0xx_fl_svd.h"
#include "fm33lc0xx_fl_aes.h"
#include "fm33lc0xx_fl_rmu.h"
#include "fm33lc0xx_fl_rng.h"
#include "fm33lc0xx_fl_opa.h"
#include "fm33lc0xx_fl_comp.h"
// #include "fm33lc0xx_fl_hdiv.h"
#include "fm33lc0xx_fl_i2c.h"
#include "fm33lc0xx_fl_spi.h"
#include "fm33lc0xx_fl_u7816.h"
// #include "fm33lc0xx_fl_bstim.h"
#include "fm33lc0xx_fl_gptim.h"
#include "fm33lc0xx_fl_atim.h"
#include "fm33lc0xx_fl_crc.h"
#include "fm33lc0xx_fl_dma.h"
#include "fm33lc0xx_fl_rtc.h"
#include "fm33lc0xx_fl_lcd.h"
#include "mf_config.h"
#include <string.h>

#define Type_bin                    0x01   // bin
#define Type_int8_t                 0x02   // int8_t
#define Type_int16_t                0x03   // int16_t
#define Type_int32_t                0x04   // int32_t
#define Type_int64_t                0x05   // int64_t
#define Type_uint8_t                0x06   // uint8_t
#define Type_uint16_t               0x07   // uint16_t
#define Type_uint32_t               0x08   // uint32_t
#define Type_uint64_t               0x09   // uint64_t
#define Type_char                   0x0A   // char
#define Type_float                  0x0B   // float
#define Type_double                 0x0C   // double
#define Type_HEXS                   0x0D   // HEXS
#define Type_ASCII                  0x0E   // ASCII
#define Type_Duration               0x0F   // Duration (uint32_t)
#define Type_Timestamp              0x10   // Timestamp
#define Type_Send_obj               0x11   // Send_obj
#define Type_Array_obj              0x12   // Array_obj
#define Type_Timeblock_obj          0x13   // Timeblock_obj
#define Type_MeterAndTargeID_obj    0x14   // MeterAndTargeID_obj
#define Type_ValveData_obj          0x15   // ValveData_obj
#define Typ_T2Ctrl_obj              0x16   // T2Ctrl_obj

enum TagName {
    positiveHeat_TAG = 0x01,        // 正向热量
    negativeHeat_TAG = 0x02,        // 反向热量
    positiveFlow_TAG = 0x03,        // 正向流量
    negativeFlow_TAG = 0x04,        // 反向流量
    positiveSpeed_TAG = 0x05,       // 正向瞬时
    negativeSpeed_TAG = 0x06,       // 反向瞬时
    positiveTemperature_TAG = 0x07, // 进水温度
    negativeTemperature_TAG = 0x08, // 回水温度
    temperatureHigh_TAG = 0x09,     // 温差(进温-回温)
    positivePressure_TAG = 0x0A,    // 进水压力
    negativePressure_TAG = 0x0B,    // 回水压力
    pressureHigh_TAG = 0x0C,        // 压差(进压-回压)
    devHigh_TAG = 0x0D,             // 深度
    powerRate_TAG = 0x0E,           // 功率
    voltage_TAG = 0x0F,             // 电压
    electricity_TAG = 0x10,         // 电流
    workTime_TAG = 0x11,            // 工作时间
    nowTime_TAG = 0x12,             // 当前时间
    CCID_TAG = 0x13,                // CCID
    IMEI_TAG = 0x14,                // IMEI
    CSQ_TAG = 0x15,                 // CSQ
    RSRP_TAG = 0x16,                // NB_DEV_RSRP
    RSRQ_TAG = 0x17,                // NB_DEV_RSRQ
    Moved_TAG = 0x18,               // Drive_Be_Moved
    SendClassObj_TAG = 0x19,        // 上传参数
    RoomTemperArray_TAG = 0x1A,     // 室温数组
    upData_TAG = 0x1B,              // 升级Code
    SendCount_TAG = 0x1C,           // 发送次数
    receiveConut_TAG = 0x1D,        // 接收次数
    SOFT_VERSION_TAG = 0x1E,        // 软件版本
    NowTemper = 0x1F,               // 当前温度
    MessageResFlag = 0x20,          // 指令回复标记
    ValveRate = 0x21,               // 开启度
    ValveFrequency = 0x22,          // 变频器频率
    FlagOfRights = 0x23,            // 权限
    IPAndProt = 0x24,               // IP和端口
    TemperSet_TAG = 0x25,           // 设定温度
    Timeblock_TAG = 0x26,           // 时段信息
    MeterAndTargeID_TAG = 0x27,     // 表号与采集器号
    NowRunMode_TAG = 0x28,          // 运行模式
    NowPressure = 0x29,             // 当前压力
    CurrentRSSI = 0x2A,             // 当前RSSI
    FrequencyPoint = 0x2B,          // 分频点
    LoarSendInterval = 0x2C,        // Loar 发送间隔
    LoarSendFlag = 0x2D,            // Loar 发送标记
    MeterType_TAG = 0x2E,           // Loar 仪表类型
    ErrorCode = 0x2F,               // 故障码
    PD1000_R = 0x30,                // PD100
    ValveSturt = 0x31,              // 阀门数据类
    SetValve_TAG = 0x32,            // 设定角度
    LimitTime_TAG = 0x33,           // 截止日期
    HotInsidTRang_TAG = 0x34,       // 热力公司规定的温度范围
    T2CtrlData_TAG = 0x35,          // 回温控制参数
    OpenValveRunTime_TAG = 0x36,    // 累计开阀时间
};
extern char TagToType[][2];
// 控制码
#define CMD_CODE_WRITE  0x80901F // 写
#define CMD_CODE_READ   0x81912F // 读
#define CMD_CODE_CTRL   0x81923F // 控制
#define CMD_CODE_RES    0x82934F // 平台回复
#define CMD_CODE_UPDATA 0x83945F // 升级

extern const char SOFT_VERSION; // 版本号
extern unsigned int UART_BAND;

#define LONG_UARTMAX   1024
typedef struct {
    char TxBuf[LONG_UARTMAX];
    uint16_t TxLen;
    char TxOpc;
    char RxBuf[LONG_UARTMAX];
    uint16_t RxLen;
    char RxOpc;
    char Rxdata;
}LongUART;

#define UARTMAX   255
typedef struct {
    char TxBuf[UARTMAX];
    uint16_t TxLen;
    char TxOpc;
    char RxBuf[UARTMAX];
    uint16_t RxLen;
    char RxOpc;
    char Rxdata;
}ShortUART;

// extern ShortUART LPUART0Ddata;
extern ShortUART LPUART0Ddata, LPUART1Ddata;

#define GPIO_SET_H(GPIOX,PINX)        FL_GPIO_SetOutputPin(GPIOX,PINX)      // 设置高电平
#define GPIO_SET_L(GPIOX,PINX)        FL_GPIO_ResetOutputPin(GPIOX,PINX)    // 设置低电平

#define EEProm_SDA_GPIO_X       GPIOA
#define EEProm_SDA_GPIO_PIN     FL_GPIO_PIN_12
#define EEprom_SDA_HIGH         GPIO_SET_H(EEProm_SDA_GPIO_X, EEProm_SDA_GPIO_PIN)
#define EEprom_SDA_LOW          GPIO_SET_L(EEProm_SDA_GPIO_X, EEProm_SDA_GPIO_PIN)  
#define EEprom_SDA_VALUE        (FL_GPIO_ReadInputPort(EEProm_SDA_GPIO_X)&EEProm_SDA_GPIO_PIN)
#define EEprom_SCL_LOW          GPIO_SET_L(GPIOA, FL_GPIO_PIN_11) 
#define EEprom_SCL_HIGH         GPIO_SET_H(GPIOA, FL_GPIO_PIN_11) 

#define Temper_NTC_PWR_HIGH     GPIO_SET_H(GPIOC,FL_GPIO_PIN_0)
#define Temper_NTC_PWR_LOW      GPIO_SET_L(GPIOC,FL_GPIO_PIN_0)
#define VOL_GND_HIGH            GPIO_SET_H(GPIOC, FL_GPIO_PIN_5)
#define VOL_GND_LOW             GPIO_SET_L(GPIOC, FL_GPIO_PIN_5)

#define Motor_PWR_HIGH          GPIO_SET_L(GPIOC, FL_GPIO_PIN_7)
#define Motor_PWR_LOW           GPIO_SET_L(GPIOC, FL_GPIO_PIN_7)
#define CloseBitValue           (FL_GPIO_ReadInputPort(GPIOB)&FL_GPIO_PIN_12)
#define CloseBitValue           (FL_GPIO_ReadInputPort(GPIOB)&FL_GPIO_PIN_12)
#define OpenBitValue            (FL_GPIO_ReadInputPort(GPIOB)&FL_GPIO_PIN_13)
#define OpenBitValue            (FL_GPIO_ReadInputPort(GPIOB)&FL_GPIO_PIN_13)

#define  GP21_PWR_HIGH          GPIO_SET_H(GPIOB, FL_GPIO_PIN_10)
#define  GP21_PWR_LOW           GPIO_SET_L(GPIOB, FL_GPIO_PIN_10) 
#define  GP21_SCLK_H            GPIO_SET_H(GPIOB, FL_GPIO_PIN_3)
#define  GP21_SCLK_L            GPIO_SET_L(GPIOB, FL_GPIO_PIN_3)
#define  GP21_SIMO_H            GPIO_SET_H(GPIOA, FL_GPIO_PIN_10)
#define  GP21_SIMO_L            GPIO_SET_L(GPIOA, FL_GPIO_PIN_10)
#define  GP21_RST_H             GPIO_SET_H(GPIOA, FL_GPIO_PIN_9)
#define  GP21_RST_L             GPIO_SET_L(GPIOA, FL_GPIO_PIN_9) 
#define  GP21_NSS_H             GPIO_SET_H(GPIOB, FL_GPIO_PIN_4)
#define  GP21_NSS_L             GPIO_SET_L(GPIOB, FL_GPIO_PIN_4) 
#define  GP21_ACLK_H            GPIO_SET_H(GPIOB, FL_GPIO_PIN_12)
#define  GP21_ACLK_L            GPIO_SET_L(GPIOB, FL_GPIO_PIN_12) 
#define  EN761_HIGH             GPIO_SET_H(GPIOB, FL_GPIO_PIN_9)
#define  EN761_LOW              GPIO_SET_L(GPIOB, FL_GPIO_PIN_9) 
#define  GP21_SOMI_VALUE        (FL_GPIO_ReadInputPort(GPIOA)&FL_GPIO_PIN_8)
#define  GP21_INT_VALUE         (FL_GPIO_ReadInputPort(GPIOB)&FL_GPIO_PIN_5)

#define Infrared_POWER_ON       GPIO_SET_H(GPIOA, FL_GPIO_PIN_15)
#define Infrared_POWER_OFF      GPIO_SET_L(GPIOA, FL_GPIO_PIN_15) 

#define M1_POWER_ON             GPIO_SET_H(GPIOB, FL_GPIO_PIN_0)
#define M1_POWER_OFF            GPIO_SET_L(GPIOB, FL_GPIO_PIN_0) 
#define BL113_IN1_HIGH          GPIO_SET_H(GPIOB, FL_GPIO_PIN_2)
#define BL113_IN1_LOW           GPIO_SET_L(GPIOB, FL_GPIO_PIN_2) 
#define BL113_IN2_HIGH          GPIO_SET_H(GPIOB, FL_GPIO_PIN_3)
#define BL113_IN2_LOW           GPIO_SET_L(GPIOB, FL_GPIO_PIN_3) 

#define Read_External_PWR       (FL_GPIO_ReadInputPort(GPIOC)&FL_GPIO_PIN_8)
#define CMT2300A_PWR_HIGH       GPIO_SET_H(GPIOC, FL_GPIO_PIN_11) 
#define CMT2300A_PWR_LOW        GPIO_SET_L(GPIOC, FL_GPIO_PIN_11)
#define CMT2300A_ReadGpio1()    (FL_GPIO_ReadInputPort(GPIOC)&FL_GPIO_PIN_10) 
#define CMT2300A_ReadGpio2()    (FL_GPIO_ReadInputPort(GPIOC)&FL_GPIO_PIN_11) 
#define CMT2300A_PWR_Out()      FL_GPIO_SetPinMode(GPIOC,FL_GPIO_PIN_3,FL_GPIO_MODE_OUTPUT)
#define CMT_SPI_FCSB_Out()      FL_GPIO_SetPinMode(GPIOC,FL_GPIO_PIN_12,FL_GPIO_MODE_OUTPUT)
#define CMT_SPI_CSB_Out()       FL_GPIO_SetPinMode(GPIOC,FL_GPIO_PIN_7,FL_GPIO_MODE_OUTPUT)
#define CMT_SPI_SCLK_Out()      FL_GPIO_SetPinMode(GPIOC,FL_GPIO_PIN_8,FL_GPIO_MODE_OUTPUT)
#define CMT_SPI_FCSB_H()        GPIO_SET_H(GPIOC,FL_GPIO_PIN_12)
#define CMT_SPI_FCSB_L()        GPIO_SET_L(GPIOC,FL_GPIO_PIN_12)
#define CMT_SPI_CSB_H()         GPIO_SET_H(GPIOC,FL_GPIO_PIN_7)
#define CMT_SPI_CSB_L()         GPIO_SET_L(GPIOC,FL_GPIO_PIN_7)
#define CMT_SPI_SCLK_H()        GPIO_SET_H(GPIOC,FL_GPIO_PIN_8)
#define CMT_SPI_SCLK_L()        GPIO_SET_L(GPIOC,FL_GPIO_PIN_8)
#define CMT_SPI_SDIO_H()        GPIO_SET_H(GPIOB, FL_GPIO_PIN_3)
#define CMT_SPI_SDIO_L()        GPIO_SET_L(GPIOB, FL_GPIO_PIN_3)
#define CMT_SPI_SDIO_Read()     (FL_GPIO_ReadInputPort(GPIOB)& FL_GPIO_PIN_3)

#define ReadKeyValue            ((FL_GPIO_ReadInputPort(GPIOC)&FL_GPIO_PIN_10))
// Define
extern void CMT_SPI_GPIO1_Set_In(void);
extern void CMT_SPI_SDIO_Set_Out(void);
extern void CMT_SPI_SDIO_Set_In(void);

#define EEPROM_AT24CXXDATA_ADDRESS 0    // AT24CxxData
#define EEPROM_OTHER_ADDRESS 184       // EEPROM_Other

#include "BSTim.h"
#include "LPTim32.h"
#include "RTC.h"
#define NowYear RTC_Date.year
#define NowMonth RTC_Date.month
#define NowDay RTC_Date.day
#define NowWeek RTC_Date.week
#define NowHour RTC_Date.hour
#define NowMinute RTC_Date.minute
#define NowSecond RTC_Date.second

#define GetDateHex(DateIemt) anyBaseToAnyBase(DateIemt, 10, 16) // 获取当前年月日的16进制值
#include "../Interflow/StrLib.h"
#include "../Interflow/PublicLib_No_One.h"
extern int floatToUint6410Rate(float Num);
extern bool HY_USB_TTL_CheckBuff(char * RxBuf, int RxLen, uint8_t Now_LPUartx);
extern void Device_Init(void);
extern void StartOPenDevMode(void);
extern void CheckMeterNum(void);

typedef enum _CodeErr {
    CODE_ERR_T2_SENSOR_FAULT_ADC,     // P 回温T2故障
    CODE_ERR_T1_SENSOR_FAULT_ADC,     // O 进温T1故障
    CODE_ERR_T2_SENSOR_FAULT_GP2,     // N 回温T2故障
    CODE_ERR_T1_SENSOR_FAULT_GP2,     // M 进温T1故障
    CODE_ERR_VALVE_OPEN_OUT_OF_RANGE, // L 测开启度数值0-100.0%越界
    CODE_ERR_VALVE_OPEN_FAULT,        // K 测开启度故障
    CODE_ERR_MOTOR_LIMIT_FAULT,       // J 电机限位故障
    CODE_ERR_WIRELESS_FAULT,          // I 无线故障 
    CODE_ERR_CARD_FAULT,              // H 刷卡故障 
    CODE_ERR_VALVE_LOW_BATTERY,       // G 阀门低电报警 
    CODE_ERR_THERMOSTAT_LOW_BATTERY,  // F 温控器低电报警  
    CODE_ERR_THERMOSTAT_LOST,         // E 温控器无线失联超2小时  
    CODE_ERR_THERMOSTAT_TAMPER,       // D 温控器被拆卸报警 
    CodeErr_yuliuC,
    CodeErr_yuliuB,
    CodeErr_yuliuA,
} CodeErr;
extern void SetErrerCode(CodeErr ErrId, bool UserSet);

typedef struct {
    uint16_t CtrlDev_Now_Volt;          // 温控电压
    uint8_t CtrlDev_NowRunMode;         // 温控运行模式
    uint16_t CtrlDev_TemperRoom;        // 室内温度
    uint16_t CtrlDev_TemperSet;         // 设定温度
    uint16_t CtrlDev_Set_Degree_Part;   // 预期阀门状态
    uint16_t CtrlDev_SendCount;         // 发送次数
    uint16_t CtrlDev_ReceiveCount;      // 接收次数
    uint8_t CtrlDev_Drive_Be_Moved;
    int CtrlDev_CMT2300A_RF_TX_dbm;
    int CtrlDev_CMT2300A_RF_RX_dbm;
    unsigned int Now_DEV_Volt;
    float Now_Temper_T1;
    float Now_Temper_T2;
    uint16_t Now_Degree_Part;       // 0 关 100 开
    uint8_t TempRunTimeForValve;    // 距离上次写入后开阀时间后的运行时间
    uint8_t FronValveDoneCode;      // 阀门动作的ID
    uint16_t RunTimeFor433;         // 433 运行时间
    uint16_t StytemErrorCode;       // 系统错误码
    bool ReceiveFlag;               // 上次通信是否接收到标记
    bool isOpen433;
} NetDevParameter;

extern NetDevParameter Get_Module_Data;    // 网络状态标记与下行指令表
#define METERID AT24CXX_Manager_NET.MeterID         // 表号
extern void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);
#endif

