#include "delay.h"

void delay_config(void)
{
    BTIM_TimeBaseInitTypeDef BTIM_TimeBaseInitStruct = {0};

    BTIM_TimeBaseInitStruct.BTIM_Mode = BTIM_MODE_TIMER;
    BTIM_TimeBaseInitStruct.BTIM_CountMode = BTIM_COUNT_MODE_REPETITIVE;
    BTIM_TimeBaseInitStruct.BTIM_Period = 0xFFFF;           // 修改：使用 uint16 最大值 65535
    BTIM_TimeBaseInitStruct.BTIM_Prescaler = 1 - 1;         // 8MHz 

    BTIM_TimeBaseInit(CW_BTIM2, &BTIM_TimeBaseInitStruct);
    BTIM_Cmd(CW_BTIM2, ENABLE);
}

/**
 * @brief  微秒级精确延时（带线性化补偿）
 * @param  us 延时微秒数 (1-1000)
 */
void delay_us(uint32_t us)
{
    if (us < 1) us = 1;
    if (us > 1000) us = 1000;
    
    // 小于20us使用_nop_()精确延时
    if (us < 20) {
        us = (us == 1) ? 1 : us >> 1;
        for (uint32_t i = 0; i < us; i++) {
            __NOP();
        }
        return;
    }
    
    // 大于等于20us使用定时器延时
    uint32_t actual_us = (DELAY_OFFSET >= us) ? 1 : (us - DELAY_OFFSET);
    uint32_t ticks = actual_us * 8;
    
    if (ticks > 0xFFFF) {
        ticks = 0xFFFF;
    }
    
    BTIM_SetCounter(CW_BTIM2, 0);
    while (BTIM_GetCounter(CW_BTIM2) < ticks);
}

/**
 * @brief  毫秒级精确延时
 * @param  ms 延时毫秒数 (0-10000)
 */
void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++) {
        delay_us(1000);  // 延时 1ms
    }
}

void delay_nop(void)
{
    __nop();
}