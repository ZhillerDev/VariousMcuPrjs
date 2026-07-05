#ifndef __TIMER_H__
#define __TIMER_H__ 

#include "global_config.h"

// ==================== 定时器标志位定义 ====================
extern volatile uint8_t u8_timer_10ms_flag;
extern volatile uint8_t u8_timer_50ms_flag;
extern volatile uint8_t u8_timer_100ms_flag;
extern volatile uint8_t u8_timer_200ms_flag;

// ==================== 定时器计数器定义 ====================
extern volatile uint8_t u8_timer_10ms_cnt;
extern volatile uint8_t u8_timer_50ms_cnt;
extern volatile uint8_t u8_timer_100ms_cnt;
extern volatile uint8_t u8_timer_200ms_cnt;

void timer_base_config(void);

#endif