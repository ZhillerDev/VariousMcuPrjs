#ifndef __PWM_H__
#define __PWM_H__ 

#include "global_config.h"

extern volatile uint16_t u16_capture_hightime;   // 高电平时间
extern volatile uint16_t u16_capture_period;     // 周期    
extern volatile uint16_t u16_capture_duty;     // 占空比
extern volatile uint16_t u16_capture_freq;     // 捕获频率

void pwm_config(void);
void pwm_breath_test(void);

void capture_config(void);
void capture_calculate(void);

#endif
