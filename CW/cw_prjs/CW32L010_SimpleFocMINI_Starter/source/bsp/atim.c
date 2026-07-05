#include "atim.h"

#include <math.h>

// 数学常量定义
#ifndef PI
#define PI 3.14159265358979323846f
#endif
#define _2PI 6.28318530717958647692f

// 约束宏：将值限制在 [low, high] 范围内
#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

// ==================== 全局变量 ====================
static float shaft_angle = 0.0f;           // 机械角度 (rad)
static uint32_t open_loop_timestamp = 0;    // 上一次时间戳 (us)
static float zero_electric_angle = 0.0f;    // 电角度偏移校准值

// 中间变量（用于调试，保留为全局）
float Ualpha, Ubeta;
float Ua, Ub, Uc;
float dc_a, dc_b, dc_c;

void atim_config(void)
{
    ATIM_InitTypeDef ATIM_InitStruct = {DISABLE, 0};
    ATIM_OCInitTypeDef ATIM_OCInitStruct = {0};

    // 配置ATIM
    __SYSCTRL_ATIM_CLK_ENABLE();
    // 对ATIM进行计数的基本设置
    ATIM_InitStruct.BufferState = DISABLE;     //不启用ARR的缓存功能
    ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_ALIGN_MODE_EDGE;    //
    ATIM_InitStruct.CounterDirection = ATIM_COUNTING_UP;
    ATIM_InitStruct.CounterOPMode = ATIM_OP_MODE_REPETITIVE;
    ATIM_InitStruct.Prescaler = 1 - 1; //8分频，计数时钟1MHz
    ATIM_InitStruct.ReloadValue = PWM_PERIOD - 1;                // 溢出周期100us
    ATIM_InitStruct.RepetitionCounter = 0;
    ATIM_Init(&ATIM_InitStruct);

    ATIM_OCInitStruct.BufferState = DISABLE;
    ATIM_OCInitStruct.OCComplement = ENABLE;
    ATIM_OCInitStruct.OCFastMode = DISABLE;
    ATIM_OCInitStruct.OCInterruptState = ENABLE;
    ATIM_OCInitStruct.OCMode = ATIM_OCMODE_PWM1;
    ATIM_OCInitStruct.OCPolarity = ATIM_OCPOLARITY_NONINVERT;
    ATIM_OCInitStruct.OCNPolarity = ATIM_OCPOLARITY_NONINVERT;
    ATIM_OC1Init(&ATIM_OCInitStruct);
    ATIM_SetCompare1(0);
    ATIM_CH1Config(ENABLE);

    ATIM_OC2Init(&ATIM_OCInitStruct);
    ATIM_SetCompare2(0);
    ATIM_CH2Config(ENABLE);

    ATIM_OC3Init(&ATIM_OCInitStruct);
    ATIM_SetCompare3(0);
    ATIM_CH3Config(ENABLE);

    ATIM_SetPWMDeadtime(20, 40, ENABLE);    // 前死区为20个单位，后死区为40个单位，死区计算见用户手册
    ATIM_CtrlPWMOutputs(ENABLE);
    // ATIM_Brake1Config(ATIM_BRAKE_POLARITY_LOW, ATIM_BRAKE_FILTER_PCLK_N8);  // brake输入信号低有效，对brake信号数字滤波，信号许连续保持8个PCLK时钟以上
    CW_ATIM->BDTR_f.AOE = 1;  //开启PWM自动输出功能，但brake等信号撤离后，可自动继续输出
    ATIM_Cmd(ENABLE);

    __disable_irq();
    NVIC_EnableIRQ(ATIM_IRQn);
    __enable_irq();
}

void ATIM_IRQHandler(void)
{
    /* USER CODE BEGIN */

    /* USER CODE END */
}

// ==================== 辅助函数 ====================

// 归一化角度到 [0, 2PI]
static float _normalizeAngle(float angle) {
    float a = fmod(angle, _2PI);
    return a >= 0 ? a : (a + _2PI);
}

// 机械角度转电角度
static float _electricalAngle(float shaft_angle, int pole_pairs) {
    return shaft_angle * pole_pairs;
}

// 获取当前微秒时间戳（需要根据CW32的定时器实现）
static uint32_t micros(void) {
    // 方案1：使用SysTick（如果已配置1us中断）
    // 方案2：使用ATIM的计数器（需要额外配置）
    // 这里提供一个简单的SysTick实现，需要你在main中初始化SysTick为1us计数
    
    // 假设你有一个全局变量 sysTick_us 在SysTick中断中递增
    extern volatile uint32_t sysTick_us;
    return sysTick_us;
}

// ==================== FOC核心函数 ====================

// 设置三相PWM占空比
static void setPwm(float Ua, float Ub, float Uc) {
    // 计算占空比并限幅 [0, 1]
    dc_a = _constrain(Ua / VOLTAGE_SUPPLY, 0.0f, 1.0f);
    dc_b = _constrain(Ub / VOLTAGE_SUPPLY, 0.0f, 1.0f);
    dc_c = _constrain(Uc / VOLTAGE_SUPPLY, 0.0f, 1.0f);
    
    // 设置PWM比较值（CW32 ATIM 比较值 = 占空比 × 周期）
    ATIM_SetCompare1((uint32_t)(dc_a * PWM_PERIOD));
    ATIM_SetCompare2((uint32_t)(dc_b * PWM_PERIOD));
    ATIM_SetCompare3((uint32_t)(dc_c * PWM_PERIOD));
}

// 逆Park+逆Clark变换，生成三相电压
static void setPhaseVoltage(float Uq, float Ud, float angle_el) {
    // 归一化电角度
    angle_el = _normalizeAngle(angle_el + zero_electric_angle);
    
    // 逆Park变换：DQ → αβ (Ud=0)
    Ualpha = -Uq * sinf(angle_el);
    Ubeta  =  Uq * cosf(angle_el);
    
    // 逆Clark变换：αβ → 三相电压（带中性点偏移至 Vcc/2）
    Ua = Ualpha + VOLTAGE_SUPPLY / 2.0f;
    Ub = (sqrtf(3.0f) * Ubeta - Ualpha) / 2.0f + VOLTAGE_SUPPLY / 2.0f;
    Uc = (-Ualpha - sqrtf(3.0f) * Ubeta) / 2.0f + VOLTAGE_SUPPLY / 2.0f;
    
    // 输出PWM
    setPwm(Ua, Ub, Uc);
}

// 开环速度控制
float velocityOpenloop(float target_velocity) {
    const float Ts = 0.001f;  // 固定1ms
    
    // 直接积分，不测量真实时间
    shaft_angle = _normalizeAngle(shaft_angle + target_velocity * Ts);
    
    float Uq = VOLTAGE_SUPPLY / 3.0f;
    setPhaseVoltage(Uq, 0.0f, _electricalAngle(shaft_angle, POLE_PAIRS));
    
    return Uq;
}