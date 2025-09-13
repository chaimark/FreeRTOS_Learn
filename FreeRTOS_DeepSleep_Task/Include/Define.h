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

extern unsigned char _Module_Start_Flage;
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
    RebootDev_TAG = 0x37,           // 重启设备
    TesTLCD_TAG = 0x38,             // 测试LCD
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

extern LongUART LPUART0Ddata, UART1Ddata;
// extern ShortUART LPUART1Ddata;

#define GPIO_SET_H(GPIOX,PINX)          FL_GPIO_SetOutputPin(GPIOX,PINX)      // 设置高电平
#define GPIO_SET_L(GPIOX,PINX)          FL_GPIO_ResetOutputPin(GPIOX,PINX)    // 设置低电平

#define GP21_NTC_PWR_HIGH               GPIO_SET_H(GPIOD, FL_GPIO_PIN_4)
#define GP21_NTC_PWR_LOW                GPIO_SET_L(GPIOD, FL_GPIO_PIN_4) 

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
extern unsigned char Cault_CS(void * Buffer, unsigned char start_num, unsigned int stop_num);
extern int Add_HY_DataClass(strnew OutPutBuff, int startAddr, unsigned char DataTAG, unsigned char VIF);

#define GetDateHex(DateIemt) anyBaseToAnyBase(DateIemt, 10, 16) // 获取当前年月日的16进制值
#include "../Interflow/StrLib.h"
#include "../Interflow/PublicLib_No_One.h"
extern int floatToint6410Rate(float Num);
extern uint8_t HY_USB_TTL_CheckBuff(char * RxBuf, int RxLen, uint8_t Now_LPUartx);
extern void Device_Init(void);
extern void StartOPenDevMode(void);
extern void CheckMeterNum(void);

typedef enum _CodeErr {
    CODE_ERR_T2_SENSOR_FAULT_ADC,     // P 回温T2故障
    CODE_ERR_T1_SENSOR_FAULT_ADC,     // O 进温T1故障
    CODE_ERR_T2_SENSOR_FAULT_GP2,     // N 回温T2故障
    CODE_ERR_T1_SENSOR_FAULT_GP2,     // M 进温T1故障
    CODE_ERR_VALVE_OPEN_OUT_OF_RANGE, // L 测开启度数值0-100.0%越界
    CODE_ERR_VALVE_OPEN_FAULT,        // K 开启度故障
    CODE_ERR_MOTOR_LIMIT_FAULT,       // J 电机限位故障
    CODE_ERR_WIRELESS_FAULT,          // I 无线故障 
    CODE_ERR_CARD_FAULT,              // H 刷卡故障 
    CODE_ERR_VALVE_LOW_BATTERY,       // G 阀门低电报警 
    yuliu2,                           // F 温控器低电报警  
    yuliu3,                           // E 温控器无线失联超2小时  
    yuliu4,                           // D 温控器被拆卸报警
    CodeErr_yuliuC,
    CodeErr_yuliuB,
    CodeErr_yuliuA,
} CodeErr;
extern void SetErrerCode(CodeErr ErrId, bool UserSet);

typedef enum _ModeStrType {
    ModeATStr,
    DownCmdStr,
    CheckStr_Task,
    CheckStr_ISR_Now,
}ModeStrType;
typedef struct {
    uint16_t CtrlDev_Set_Degree_Part;   // 预期阀门状态
    unsigned int Now_DEV_Volt;          // 当前电压
    float Now_Temper_T1;                // 当前 T1
    float Now_Temper_T2;                // 当前 T2
    uint16_t Now_Degree_Part;           // 0 关 1000 开
    uint8_t TempRunTimeForValve;        // 当前阀门大于 40% 的状态下的运行时间
    uint8_t FronValveDoneCode;          // 上一次阀门动作的 ID
    uint16_t FrontValveDegree_Part;     // 上一次阀门动作的角度
    uint16_t StytemErrorCode;           // 系统错误码
    bool ReceiveFlag;                   // 上次通信是否接收到标记
    bool isUpCode;                      // 是否升级
    bool isPowerModuleUseing;           // 是否有模组在使用电源
    struct {
        bool isSendOk;                      // 是否发送成功
        uint8_t isWriteEEprom;              // 是否写eeprom
        int8_t NowNetOnlineFlag;            // 网络在线标记
        unsigned char isGetDownCmd;         // 是否定时主动获取指令
        char * checkFlagStr;                // 主动获取指令时模组主动推出的提示字符串
        void (*GetDownCmd)(void);           // 获取下行指令
        bool (*SendData)(void);             // 发送数据
        ModeStrType(*UartCmdType)(void);
        uint16_t Special_ID;                // 特殊处理的 ID
    }Now_NetDevParameter;
} NetDevParameter;
extern NetDevParameter System_RunData;    // 网络状态标记与下行指令表

#include "../Interflow/StrLib.h"
// typedef struct _MeterData {
//     char DataStartHour;         // 数据的开始点
//     char DataCount;             // 数据的个数
//     struct _MeterTemperature {
//         double RoomTemperature;     // 室内温度 Clock_InsideTH & Clock_InsideTL
//         char CollectionTime[20];    // 采集时间 (yyyy-MM-dd HH:mm:ss)
//     }MeterTemperature[24];          // 24小时温度数据
//     void (*EnterQueue)(struct _MeterData This, double Temperature);     // 入队函数
//     double (*DeleteQueue)(struct _MeterData This);   // 出队函数
// }MeterData;
// extern void _EnterQueue(struct _MeterData This, double Temperature);
// extern double _DeleteQueue(struct _MeterData This);
// extern MeterData Meter_Manager;

#define METERID AT24CXX_Manager_NET.MeterID         // 表号
extern void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);
#endif

