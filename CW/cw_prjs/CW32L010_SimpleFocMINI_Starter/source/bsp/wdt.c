#include "wdt.h"

void iwdt_config(void)
{
    IWDT_InitTypeDef IWDT_InitStruct = {0};

    IWDT_DeInit();

    IWDT_InitStruct.IWDT_ITState = ENABLE;
    IWDT_InitStruct.IWDT_OverFlowAction = IWDT_OVERFLOW_ACTION_INT;
    IWDT_InitStruct.IWDT_Pause = IWDT_SLEEP_PAUSE;
    IWDT_InitStruct.IWDT_Prescaler = IWDT_Prescaler_DIV512;    // IWDT使用LSI为计时时钟，分频后计时间隔为15.6ms
    IWDT_InitStruct.IWDT_ReloadValue = IWDT_2_SECS;  // 2s
    IWDT_InitStruct.IWDT_WindowValue = 0xFFF;

    IWDT_Init(&IWDT_InitStruct);

    IWDT_Cmd();
    while(!CW_IWDT->SR_f.RUN);
    IWDT_Refresh();

    __disable_irq();
    NVIC_EnableIRQ(WDT_IRQn);
    __enable_irq();
}

void iwdt_feed(void)
{
    IWDT_Refresh();
}