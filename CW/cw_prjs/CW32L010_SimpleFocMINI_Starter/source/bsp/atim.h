#ifndef __ATIM_H__
#define __ATIM_H__

#include "global_config.h"

// ==================== 电机参数配置 ====================
#define POLE_PAIRS       7          // 极对数（2804电机为7对极）
#define VOLTAGE_SUPPLY   12.6f      // 电源电压 (V)
#define PWM_PERIOD       375        // PWM频率 30kHz
#define TARGET_VELOCITY  10.0f       // 目标角速度 (rad/s)

void atim_config(void);

void motor_test(void);

float velocityOpenloop(float target_velocity);

#endif
