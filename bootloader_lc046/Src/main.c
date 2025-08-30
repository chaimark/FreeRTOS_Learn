/**
  ****************************************************************************************************
  * @file    main.c
  * @author  FMSH Application Team
  * @brief   Header file of FL Module
  ****************************************************************************************************
  * @attention
  *
  * Copyright (c) [2021] [Fudan Microelectronics]
  * THIS SOFTWARE is licensed under Mulan PSL v2.
  * can use this software according to the terms and conditions of the Mulan PSL v2.
  * You may obtain a copy of Mulan PSL v2 at:
  *          http://license.coscl.org.cn/MulanPSL2
  * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
  * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
  * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
  * See the Mulan PSL v2 for more details.
  *
  ****************************************************************************************************
  */

#include "main.h"
#include "fm33_assert.h"



/**
* Chip Series: FM33LC0xx;
* FL Version: v2.3;
* Project Name: gateway;
* Project Version: v2.3.0.0;
* Create Date: 2021-11-16;
*/

int main(void) {
    /* Configure the system clock */
    MF_Clock_Init();

    /* Initialize FL Driver Library */
    FL_Init();

    /* Initialize all configured peripherals */
    MF_Config_Init();

    // ****************************************
    // ****************************************

    // 启动定时器
    // BSTIM_enable();

    FL_IWDT_ReloadCounter(IWDT);    // 喂狗
    // 功能初始化
    //RS485_SEND;
    FL_DelayMs(1000);
    console_init();
    FL_DelayMs(1000);

    FL_IWDT_ReloadCounter(IWDT);    // 喂狗
    if (updataCheck() < 0) {
        printf("no updata\r\n");
    } else {
        if (updataCopyProgram() < 0) {
            printf("updata error\r\n");
        } else {
            printf("updata sucess and jump to app\r\n");
            // BSTIM_disable();
        }
    }
    updataInit(); // 复位升级标记区
    uint32_t JumpAddress;
    JumpAddress = *(__IO uint32_t *) (UPDATA_MCU_OFFSET + 4);
    __set_MSP(*(__IO uint32_t *) UPDATA_MCU_OFFSET);
    (*(void (*)())JumpAddress) ();

    printf("bootloader error\r\n");
    while (1) {
        FL_IWDT_ReloadCounter(IWDT);    // 喂狗
        //console_task();
        FL_DelayMs(10);
    }
}


