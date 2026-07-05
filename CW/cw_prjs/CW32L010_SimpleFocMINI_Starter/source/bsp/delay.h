#ifndef __DELAY_H__
#define __DELAY_H__

#include "global_config.h"

#define US_TO_TICKS(us) ((us) * 8) // 8MHz时钟，每微秒8个时钟周期

#define DELAY_OFFSET    10 // 预估函数调用和指令执行的时间，单位为微秒

// 适用于CortexM0+架构的延时函数
void delay_config(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void delay_nop(void);

#endif
