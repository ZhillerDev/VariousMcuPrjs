#include "timer.h"

#define TIMER_TEST_ORDER 1  // 0:不做任何测试，1:测试IO翻转

// ==================== 定时器标志位定义 ====================
volatile uint8_t u8_timer_10ms_flag = 0;
volatile uint8_t u8_timer_50ms_flag = 0;
volatile uint8_t u8_timer_100ms_flag = 0;
volatile uint8_t u8_timer_200ms_flag = 0;

// ==================== 定时器计数器定义 ====================
volatile uint8_t u8_timer_10ms_cnt = 0;
volatile uint8_t u8_timer_50ms_cnt = 0;
volatile uint8_t u8_timer_100ms_cnt = 0;
volatile uint8_t u8_timer_200ms_cnt = 0;

void timer_base_config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    BTIM_TimeBaseInitTypeDef BTIM_TimeBaseInitStruct = {0}; // 定义定时器初始化结构体

    // 初始化BTIM1为1ms周期的定时器
    BTIM_DeInit(CW_BTIM1);                            // 复位BTIM1外设

    BTIM_TimeBaseInitStruct.BTIM_Mode = BTIM_MODE_TIMER; // 设置为定时器模式
    BTIM_TimeBaseInitStruct.BTIM_Prescaler = 8 - 1;   // 8分频得到1MHz计数频率
    BTIM_TimeBaseInitStruct.BTIM_Period = 1000 - 1;   // 计数1000次产生一次中断(1ms)

    BTIM_TimeBaseInit(CW_BTIM1, &BTIM_TimeBaseInitStruct); // 初始化定时器

    BTIM_ITConfig(CW_BTIM1, BTIM_IT_UPDATE, ENABLE);   // 使能更新中断

    BTIM_Cmd(CW_BTIM1, ENABLE);                        // 开启定时器

    __disable_irq();                                   // 关闭总中断
    NVIC_EnableIRQ(BTIM1_IRQn);                        // 开启NVIC中断通道
    __enable_irq();                                    // 开启总中断

#if TIMER_TEST_ORDER == 1
    // 初始化PA2为输出模式（用于测试）
    GPIO_InitStruct.IT = GPIO_IT_NONE;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pins = GPIO_PIN_2;
    GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
#endif
}

void BTIM1_IRQHandler(void)
{
    /* USER CODE BEGIN */
    if (BTIM_GetITStatus(CW_BTIM1, BTIM_IT_UPDATE))
    {
        BTIM_ClearITPendingBit(CW_BTIM1, BTIM_IT_UPDATE);

        // velocityOpenloop(TARGET_VELOCITY);
        
        // 10ms计数器处理
        if (++u8_timer_10ms_cnt >= 10)  // 10 * 1ms = 10ms
        {
            u8_timer_10ms_cnt = 0;
            u8_timer_10ms_flag = 1;
        }
        
        // 50ms计数器处理
        if (++u8_timer_50ms_cnt >= 50)  // 50 * 1ms = 50ms
        {
            u8_timer_50ms_cnt = 0;
            u8_timer_50ms_flag = 1;
        }
        
        // 100ms计数器处理
        if (++u8_timer_100ms_cnt >= 100)  // 100 * 1ms = 100ms
        {
            u8_timer_100ms_cnt = 0;
            u8_timer_100ms_flag = 1;
        }
        
        // 200ms计数器处理
        if (++u8_timer_200ms_cnt >= 200)  // 200 * 1ms = 200ms
        {
            u8_timer_200ms_cnt = 0;
            u8_timer_200ms_flag = 1;
        }
    }

    /* USER CODE END */
}