#ifndef __DISPLAY_LCD_LIB_H__
#define __DISPLAY_LCD_LIB_H__
extern unsigned int Display_Buffer[8];
/*
//0~9, A,B, C, D, E, F, 空
const uint8_t FONT_CODE_TABLE[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,
    0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x00,
    0x7D, 0x38, 0x37, 0x3F, 0x73, 0x3E, 0x6D,//G,L,N,O,P,U,S
    0x5c, 0x50, 0x01, 0x40, 0x08, 0x41, 0x09, 0x48, 0x49,0x00,0x54,0x76,0x47,0x40,0x76,//xiao_o,xiao_r,LINE1,LINE2,LINE3,LINE12,LINE13,LINE23,LINE123,SPACE,xiao_n,da_h,fan_f,mid_line,xiao_h
};
*/
#define   da_I    1
#define   da_A    10
#define   da_B    11
#define   da_C    12
#define   da_D    13
#define   da_E    14
#define   da_F    15
#define   SPACE   16
#define   da_G    17 
#define   da_L    18 
#define   da_N    19 
#define   da_O    20
#define   da_P    21 
#define   da_U    22
#define   da_S    23
#define   xiao_o  24 
#define   xiao_r  25
#define   LINE1   26
#define   LINE2   27
#define   LINE3   28
#define   LINE12  29
#define   LINE13  30
#define   LINE23  31
#define   LINE123 32

//BIT MAP======================================================
#define BIT0            0x00000001
#define BIT1            0x00000002
#define BIT2            0x00000004
#define BIT3            0x00000008
#define BIT4            0x00000010
#define BIT5            0x00000020
#define BIT6            0x00000040
#define BIT7            0x00000080
#define BIT8            0x00000100
#define BIT9            0x00000200
#define BIT10           0x00000400
#define BIT11           0x00000800
#define BIT12           0x00001000
#define BIT13           0x00002000
#define BIT14           0x00004000
#define BIT15           0x00008000
#define BIT16           0x00010000
#define BIT17           0x00020000
#define BIT18           0x00040000
#define BIT19           0x00080000
#define BIT20           0x00100000
#define BIT21           0x00200000
#define BIT22           0x00400000
#define BIT23           0x00800000
#define BIT24           0x01000000
#define BIT25           0x02000000
#define BIT26           0x04000000
#define BIT27           0x08000000
#define BIT28           0x10000000
#define BIT29           0x20000000
#define BIT30           0x40000000
#define BIT31           0x80000000
#define BITNO           0x00000000

// 功能 COM SEG    
#define LCD_ChineseOpen_VALVE           Display_Buffer[0] |= BIT0  // 开阀      0 9 ； 左移 0 位
#define LCD_ChineseClose_VALVE          Display_Buffer[1] |= BIT0  // 关阀      1 9 ； 左移 0 位
#define LCD_TimeDate                    Display_Buffer[2] |= BIT0  // 日期      2 9 ； 左移 0 位
#define LCD_Meter_ID                    Display_Buffer[3] |= BIT0  // 表号      3 9 ； 左移 0 位
#define LCD_CAIJIQI_ID                  Display_Buffer[4] |= BIT0  // 采集器    4 9 ； 左移 0 位
#define LCD_ChineseRest                 Display_Buffer[5] |= BIT0  // 剩余      5 9 ； 左移 0 位
#define LCD_BATTERYX1                   Display_Buffer[6] |= BIT0  // X1        6 9 ； 左移 0 位
#define LCD_BATTERYX2                   Display_Buffer[7] |= BIT0  // X2        7 9 ； 左移 0 位
#define LCD_AlarmSign                   Display_Buffer[0] |= BIT1  // T5        0 10 ； 左移 1 位
#define LCD_SINGAL                      Display_Buffer[1] |= BIT1  // T6        1 10 ； 左移 1 位
#define LCD_Temper_Sign                 Display_Buffer[2] |= BIT1  // ℃        2 10 ； 左移 1 位
#define LCD_GJ_Sign                     Display_Buffer[3] |= BIT1  // GJ        3 10 ； 左移 1 位
#define LCD_Kw_Hour_Sign                Display_Buffer[4] |= BIT1  // T1        4 10 ； 左移 1 位
#define LCD_Kw_Sign                     Display_Buffer[5] |= BIT1  // KW        5 10 ； 左移 1 位
#define LCD_BATT_BACK                   Display_Buffer[6] |= BIT1  // X5        6 10 ； 左移 1 位
#define LCD_BATTERYX4                   Display_Buffer[7] |= BIT1  // X4        7 10 ； 左移 1 位
#define LCD_BATTERYX3                   Display_Buffer[7] |= BIT2  // X3        7 11 ； 左移 2 位
// #define LCD_ChineseRoomTemper           Display_Buffer[0] |= BIT4  // T2        0 13 ； 左移 4 位
// #define LCD_PleasePay                   Display_Buffer[1] |= BIT4  // T3        1 13 ； 左移 4 位
// #define LCD_ChineseHeat                 Display_Buffer[2] |= BIT4  // 热量      2 13 ； 左移 4 位
// #define LCD_ChineseSet                  Display_Buffer[3] |= BIT4  // T4        3 13 ； 左移 4 位
// #define LCD_ChineseTemper               Display_Buffer[4] |= BIT4  // T7        4 13 ； 左移 4 位
// #define LCD_COL                         Display_Buffer[5] |= BIT4  // COL       5 13 ； 左移 4 位
// #define LCD_Time                        Display_Buffer[7] |= BIT4  // 时钟      7 13 ； 左移 4 位

#define NO_LCD_ChineseOpen_VALVE        Display_Buffer[0] &= ~BIT0  // 开阀      0 9 ； 左移 0 位
#define NO_LCD_ChineseClose_VALVE       Display_Buffer[1] &= ~BIT0  // 关阀      1 9 ； 左移 0 位
#define NO_LCD_TimeDate                 Display_Buffer[2] &= ~BIT0  // 日期      2 9 ； 左移 0 位
#define NO_LCD_Meter_ID                 Display_Buffer[3] &= ~BIT0  // 表号      3 9 ； 左移 0 位
#define NO_LCD_CAIJIQI_ID               Display_Buffer[4] &= ~BIT0  // 采集器    4 9 ； 左移 0 位
#define NO_LCD_ChineseRest              Display_Buffer[5] &= ~BIT0  // 剩余      5 9 ； 左移 0 位
#define NO_LCD_BATTERYX1                Display_Buffer[6] &= ~BIT0  // X1        6 9 ； 左移 0 位
#define NO_LCD_BATTERYX2                Display_Buffer[7] &= ~BIT0  // X2        7 9 ； 左移 0 位
#define NO_LCD_AlarmSign                Display_Buffer[0] &= ~BIT1  // T5        0 10 ； 左移 1 位
#define NO_LCD_SINGAL                   Display_Buffer[1] &= ~BIT1  // T6        1 10 ； 左移 1 位
#define NO_LCD_Temper_Sign              Display_Buffer[2] &= ~BIT1  // ℃        2 10 ； 左移 1 位
#define NO_LCD_GJ_Sign                  Display_Buffer[3] &= ~BIT1  // GJ        3 10 ； 左移 1 位
#define NO_LCD_Kw_Hour_Sign             Display_Buffer[4] &= ~BIT1  // T1        4 10 ； 左移 1 位
#define NO_LCD_Kw_Sign                  Display_Buffer[5] &= ~BIT1  // KW        5 10 ； 左移 1 位
#define NO_LCD_BATT_BACK                Display_Buffer[6] &= ~BIT1  // X5        6 10 ； 左移 1 位
#define NO_LCD_BATTERYX4                Display_Buffer[7] &= ~BIT1  // X4        7 10 ； 左移 1 位
#define NO_LCD_BATTERYX3                Display_Buffer[7] &= ~BIT2  // X3        7 11 ； 左移 2 位
// #define NO_LCD_ChineseRoomTemper        Display_Buffer[0] &= ~BIT4  // T2        0 13 ； 左移 4 位
// #define NO_LCD_PleasePay                Display_Buffer[1] &= ~BIT4  // T3        1 13 ； 左移 4 位
// #define NO_LCD_ChineseHeat              Display_Buffer[2] &= ~BIT4  // 热量      2 13 ； 左移 4 位
// #define NO_LCD_ChineseSet               Display_Buffer[3] &= ~BIT4  // T4        3 13 ； 左移 4 位
// #define NO_LCD_ChineseTemper            Display_Buffer[4] &= ~BIT4  // T7        4 13 ； 左移 4 位
// #define NO_LCD_COL                      Display_Buffer[5] &= ~BIT4  // COL       5 13 ； 左移 4 位
// #define NO_LCD_Time                     Display_Buffer[7] &= ~BIT4  // 时钟      7 13 ； 左移 4 位

extern void LcdDisplayRefresh(void);
extern void ClearDisplayAll(void);
extern void MF_LCD_Init(void);
extern void LCD_Num_Data(uint8_t Show, uint8_t Num);
extern void LCD_SetDotPx(uint8_t DotNum, bool isShow);

#endif
