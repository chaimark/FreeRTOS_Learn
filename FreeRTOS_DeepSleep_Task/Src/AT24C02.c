#include "fm33lc0xx_fl.h"
#include "AT24C02.h"
#include "Define.h"
#include "NumberBaseLib.h"
#include "RTC_SetTime.h"
#include "PublicLib_No_One.h"

int HY_FLAG = true;
// public
AT24CXX_MANAGER_T AT24CXX_Manager = {
    .Sing = 0xB2,
    .MeterID = {0x12, 0x34, 0x56, 0x78},
    .ModeCode = 0x3FFF,
    .InsetT_Max = 100,
    .InsetT_Min = 0,
    .LimitTime = {"1999-05-20 13:14:00"},
    .RunTimeBack_AddrNum = 0,
    .T2AutoCtrlBuf = {0},
    .Time_Data = {0},
};

// void SaveDevData(void) {
//     unsigned int HourInt = 0;
//     HourInt = (NowHour / 0x10) * 10 + (NowHour % 0x10);
//     if (HourInt < 24) {
//         // Meter_Manager.EnterQueue(&Meter_Manager, Get_Module_Data.Now_Temper_T1);        // 当前温度为正值
//     }
// }
#define TRUE        1
#define FALSE       0
#define AckError    0x55

unsigned char  EEprom_Err_Flage;   //0:正常   1：错误
/**
  * @brief  I2C_MASTER Initialization function
  * @param  void
  * @retval None
  */
void MF_I2C_MASTER_Init(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct;
    //PA11_SCLK   PA12_SDATA   高阻
    GPIO_InitStruct.pin = FL_GPIO_PIN_11 | EEProm_SDA_GPIO_PIN;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    GPIO_InitStruct.remapPin = FL_DISABLE;
    FL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SET_H(GPIOA, FL_GPIO_PIN_11 | EEProm_SDA_GPIO_PIN);
}
static void EEprom_SDA_IN(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = EEProm_SDA_GPIO_PIN;
    GPIO_InitStruct.mode = FL_GPIO_MODE_INPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(EEProm_SDA_GPIO_X, &GPIO_InitStruct);
}

static void EEprom_SDA_OUT(void) {
    FL_GPIO_InitTypeDef    GPIO_InitStruct = {0};
    GPIO_InitStruct.pin = EEProm_SDA_GPIO_PIN;
    GPIO_InitStruct.mode = FL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.outputType = FL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.pull = FL_DISABLE;
    FL_GPIO_Init(EEProm_SDA_GPIO_X, &GPIO_InitStruct);
    GPIO_SET_H(EEProm_SDA_GPIO_X, EEProm_SDA_GPIO_PIN);
}

void IIC_Start(void) {
    EEprom_SDA_OUT();
    FL_DelayUs(4);
    EEprom_SDA_HIGH;
    FL_DelayUs(4);
    EEprom_SCL_HIGH;
    FL_DelayUs(4);
    EEprom_SDA_LOW;
    FL_DelayUs(4);
    EEprom_SCL_LOW;
    FL_DelayUs(4);
}

void IIC_Stop(void) {
    EEprom_SDA_OUT();
    EEprom_SDA_LOW;
    FL_DelayUs(4);
    EEprom_SCL_HIGH;
    FL_DelayUs(4);
    EEprom_SDA_LOW;
    FL_DelayUs(4);
    EEprom_SDA_HIGH;
}

void IIC_SendAck(void) {
    EEprom_SDA_OUT();
    EEprom_SDA_LOW;
    FL_DelayUs(4);
    EEprom_SCL_LOW;
    FL_DelayUs(4);
    EEprom_SCL_HIGH;
    FL_DelayUs(4);
    EEprom_SCL_LOW;
    FL_DelayUs(4);
    EEprom_SDA_HIGH;
}

void IIC_SendNoAck(void) {
    EEprom_SDA_OUT();
    EEprom_SDA_HIGH;
    FL_DelayUs(4);
    EEprom_SCL_LOW;
    FL_DelayUs(4);
    EEprom_SCL_HIGH;
    FL_DelayUs(4);
    EEprom_SCL_LOW;
}

unsigned char IIC_Check_Ack(void) {
    unsigned char AckStatus;
    EEprom_SDA_IN();
    EEprom_SCL_HIGH;
    FL_DelayUs(4);
    if (EEprom_SDA_VALUE != 0) {
        AckStatus = FALSE;
    } else {
        AckStatus = TRUE;
    }
    EEprom_SCL_LOW;
    FL_DelayUs(4);
    EEprom_SDA_OUT();
    return AckStatus;
}

void IIC_SendByte(unsigned char data) {
    unsigned char tmp;
    EEprom_SDA_OUT();
    for (tmp = 0; tmp < 8; tmp++) {
        if (data & 0x80) {
            EEprom_SDA_HIGH;
        } else {
            EEprom_SDA_LOW;
        }
        FL_DelayUs(4);
        EEprom_SCL_HIGH;
        FL_DelayUs(4);
        EEprom_SCL_LOW;
        FL_DelayUs(4);
        data <<= 1;
    }
    FL_DelayUs(4);
}

unsigned char IIC_RevByte(void) {
    unsigned char tmp;
    unsigned char DATA = 0;
    EEprom_SDA_IN();
    EEprom_SCL_LOW;
    FL_DelayUs(4);
    for (tmp = 0; tmp < 8; tmp++) {
        EEprom_SCL_HIGH;
        FL_DelayUs(4);
        DATA <<= 1;
        if (EEprom_SDA_VALUE != 0) {
            DATA |= 0x01;
        } else {
            DATA &= 0xfe;
        }
        EEprom_SCL_LOW;
    }
    EEprom_SDA_OUT();
    return DATA;
}

unsigned char EEprom_Byte1Write_No_Check(unsigned char addr, unsigned char data) {
    unsigned char Dev_addr;
    Dev_addr = 0xa0;
    IIC_Start();
    IIC_SendByte(Dev_addr);
    if (IIC_Check_Ack() == FALSE) {
        IIC_Stop();
        IncludeDelayMs(1);
        return AckError;
    }
    IIC_SendByte(addr);
    if (IIC_Check_Ack() == FALSE) {
        IIC_Stop();
        IncludeDelayMs(1);
        return AckError;
    }
    IIC_SendByte(data);
    if (IIC_Check_Ack() == FALSE) {
        IIC_Stop();
        IncludeDelayMs(1);
        return AckError;
    }
    IIC_Stop();
    IncludeDelayMs(1);
    return 0;
}

unsigned char EEprom_Byte1Read_No_Check(unsigned char addr) {
    unsigned char data;
    unsigned char Dev_addr;
    Dev_addr = 0xa0;
    IIC_Start();
    IIC_SendByte(Dev_addr);
    if (IIC_Check_Ack() == FALSE) {
        IIC_Stop();
        IncludeDelayMs(1);
        return AckError;
    }
    FL_DelayUs(1);
    IIC_SendByte(addr);
    if (IIC_Check_Ack() == FALSE) {
        IIC_Stop();
        IncludeDelayMs(1);
        return AckError;
    }
    FL_DelayUs(1);
    IIC_Start();
    Dev_addr = 0xa1;
    IIC_SendByte(Dev_addr);
    if (IIC_Check_Ack() == FALSE) {
        IIC_Stop();
        IncludeDelayMs(1);
        return AckError;
    }
    FL_DelayUs(1);
    data = IIC_RevByte();
    IIC_SendNoAck();
    IIC_Stop();
    IncludeDelayMs(1);
    return data;
}

unsigned char EEprom_Byte1Read(unsigned char addr) {
    unsigned char data[3] = {0};
    unsigned char i;
    EEprom_Err_Flage = 0; //清除错误标识
    for (i = 0; i < 2; i++)
        data[i] = EEprom_Byte1Read_No_Check(addr);
    if (data[0] == data[1])
        return data[0];
    else {
        data[2] = EEprom_Byte1Read_No_Check(addr);
        if (data[0] == data[2])
            return data[0];
        else if (data[1] == data[2])
            return data[1];
        else {
            EEprom_Err_Flage = 1; //错误标识
            return AckError;
        }
    }
}

unsigned char EEprom_Byte1Write(unsigned char addr, unsigned char data) {
    unsigned char i;
    EEprom_Err_Flage = 0; //清除错误标识
    for (i = 0; i < 3; i++) {
        EEprom_Byte1Write_No_Check(addr, data);
        if (EEprom_Byte1Read(addr) == data)
            return EEprom_Err_Flage;  //写完读出校验正确
    }
    EEprom_Err_Flage = 1;       //3次都不正确, 标记错误
    return EEprom_Err_Flage;
}

void EEprom_Byte2Write(unsigned char addr, unsigned int data) {
    unsigned char i, j;
    unsigned char err[2] = {0};
    i = data / 256;
    j = data % 256;
    err[0] = EEprom_Byte1Write(addr, i);
    err[1] = EEprom_Byte1Write(addr + 1, j);
    if ((err[0] == 0) && (err[1] == 0))
        EEprom_Err_Flage = 0; //正常
    else
        EEprom_Err_Flage = 1;                         //故障失败 
}

unsigned int EEprom_Byte2Read(unsigned char addr) {
    unsigned int EE_Data2Byte;
    EE_Data2Byte = EEprom_Byte1Read(addr);
    if (EEprom_Err_Flage != 0)
        return EEprom_Err_Flage;//失败
    EE_Data2Byte = EE_Data2Byte * 256;
    EE_Data2Byte = EE_Data2Byte + EEprom_Byte1Read(addr + 1);
    if (EEprom_Err_Flage != 0)
        return EEprom_Err_Flage;//失败
    else
        return EE_Data2Byte;
}


///*****************************************///
/**
 * isWriteFlag == true 强制写 eeprom
*/
unsigned char EEprom_AT24CXX_Parameter_Init(bool isWriteFlag) {
    unsigned char ret;
    int len = sizeof(AT24CXX_Manager);
    unsigned char * p = (unsigned char *)(&AT24CXX_Manager);
    if ((EEprom_Byte1Read(EEPROM_AT24CXXDATA_ADDRESS) != AT24CXX_Manager.Sing) || (isWriteFlag)) {
        for (int i = 1; i < len; i++) {
            ret = EEprom_Byte1Write(i + EEPROM_AT24CXXDATA_ADDRESS, p[i]);
            if (ret != 0) {
                // printf("e2 err1\r\n");
                return ret;
            }
        }
        ret = EEprom_Byte1Write(EEPROM_AT24CXXDATA_ADDRESS, p[0]);
        if (ret != 0) {
            // printf("e2 err2\r\n");
            return ret;
        }
    } else {
        for (int i = 1; i < len; i++) {
            p[i] = EEprom_Byte1Read(i);
        }
    }
    // printf("e2 ok\r\n");
    return 0;
}

// 写参数 位置
unsigned char EEprom_Write_Start(unsigned char * data, unsigned char addr, unsigned char len) {
    int i;
    unsigned char ret;
    for (i = 0; i < len; i++) {
        ret = EEprom_Byte1Write(addr + i, data[i]);
        if (ret != 0) {
            //printf("e2 err3\r\n");
            return ret;
        }
    }
    return 0;
}
// 写参数
unsigned char EEprom_AT24C0XXData_Write(void * data, unsigned char len) {
    char addr = (int)data - (int)(&AT24CXX_Manager); // 数据地址
    return EEprom_Write_Start((unsigned char *)data, EEPROM_AT24CXXDATA_ADDRESS + addr, len);
}
// 读参数 位置
unsigned char EEprom_Read_Start(unsigned char * Buff, unsigned short int addr, unsigned char len) {
    for (int i = 0; i < len; i++) {
        Buff[i] = EEprom_Byte1Read(addr + i);
    }
    return 0;
}
// 读参数
unsigned char EEprom_AT24C0XXData_Read(unsigned char * data, unsigned char size) {
    char addr = (int)data - (int)(&AT24CXX_Manager); // 数据地址
    return EEprom_Read_Start(data, EEPROM_AT24CXXDATA_ADDRESS + addr, size);
}
void ReadTime_EEprom(FL_RTC_InitTypeDef * SpaceBuff) {
    char addr = (int)&AT24CXX_Manager.Time_Data - (int)(&AT24CXX_Manager);
    EEprom_Read_Start((unsigned char *)SpaceBuff, EEPROM_AT24CXXDATA_ADDRESS + addr, sizeof(FL_RTC_InitTypeDef));
}

