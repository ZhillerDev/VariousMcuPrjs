#ifndef __GLOBAL_CONFIGS_H__
#define __GLOBAL_CONFIGS_H__

//=============================================
//C语言标准库
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

//=============================================
//全局定义
#include "global_variables.h"

//=============================================
//CW_LIBS 标准固件库
#include "interrupts_cw32l010.h"
#include "base_types.h"
#include "cw32l010.h"
#include "system_cw32l010.h"
#include "cw32l010_systick.h"
#include "cw32l010_sysctrl.h"
#include "cw32l010_gpio.h"
#include "cw32l010_atim.h"
#include "cw32l010_btim.h"
#include "cw32l010_gtim.h"
#include "cw32l010_lptim.h"
#include "cw32l010_uart.h"
#include "cw32l010_iwdt.h"
#include "cw32l010_adc.h"
#include "cw32l010_rtc.h"
#include "cw32l010_spi.h"


// 定义位域结构体
typedef union
{
    uint8_t byte; // 整体访问

    struct
    {
        uint8_t bit0 : 1; // 最低位
        uint8_t bit1 : 1;
        uint8_t bit2 : 1;
        uint8_t bit3 : 1;
        uint8_t bit4 : 1;
        uint8_t bit5 : 1;
        uint8_t bit6 : 1;
        uint8_t bit7 : 1; // 最高位
    } bits;

} BYTE;



#endif
