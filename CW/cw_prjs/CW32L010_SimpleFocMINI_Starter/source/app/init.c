#include "init.h"

void mcu_init(void)
{
    init_variables();
    init_system();
    init_gpios();
    init_peripherals();
}

static void init_system(void)
{
    SYSCTRL_HSI_Enable(SYSCTRL_HSIOSC_DIV6);        // SYSCLK = 48MHZ / 6 = 8MHZ
    SYSCTRL_LSI_Enable();                           // 使能LSI时钟
    SYSCTRL_HCLKPRS_Config(SYSCTRL_HCLK_DIV1);      // HCLK = 8MHZ 不分频
    SYSCTRL_PCLKPRS_Config(SYSCTRL_PCLK_DIV1);      // PCLK = 8MHZ 不分频

    SYSCTRL_SystemCoreClockUpdate(8000000);         // 更新系统时钟
    SysTick_Config(SystemCoreClock / 1000);         // 配置SysTick为1ms中断周期，1khz    

    // 使能GPIOB和UART2时钟
    SYSCTRL_AHBPeriphClk_Enable(SYSCTRL_AHB_PERIPH_GPIOB, ENABLE);
    SYSCTRL_APBPeriphClk_Enable1(SYSCTRL_APB1_PERIPH_UART2, ENABLE);

    __SYSCTRL_IWDT_CLK_ENABLE();                    // 使能IWDT模块
    __SYSCTRL_BTIM123_CLK_ENABLE();                 // 使能BTIM1/2/3时钟
    __SYSCTRL_LPTIM_CLK_ENABLE();                   // 使能LPTIM时钟
    __SYSCTRL_GTIM1_CLK_ENABLE();                   // 使能GTIM1时钟
    __SYSCTRL_ATIM_CLK_ENABLE();                    // 使能ATIM时钟
    __SYSCTRL_ADC_CLK_ENABLE();                     // 使能ADC时钟
    __SYSCTRL_GPIOB_CLK_ENABLE();                   // 使能GPIOB时钟
    __SYSCTRL_GPIOA_CLK_ENABLE();                   // 使能GPIOA时钟

    
}

static void init_peripherals(void)
{
    delay_config();                                    // 配置延时函数

    iwdt_config();
    timer_base_config();
    uart_config();
    atim_config();
}

static void init_gpios(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    //=============================================
    // 配置FOC PWM输出引脚：PB00/PB01/PA03
    PB00_AFx_ATIMCH1();
    GPIO_InitStruct.IT = GPIO_IT_NONE;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pins = GPIO_PIN_0;
    GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    PB01_AFx_ATIMCH2();
    GPIO_InitStruct.IT = GPIO_IT_NONE;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pins = GPIO_PIN_1;
    GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    PA03_AFx_ATIMCH3();
    GPIO_InitStruct.IT = GPIO_IT_NONE;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pins = GPIO_PIN_3;
    GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

    //=============================================
    // 配置FOC驱动芯片使能引脚：PA04
    PA04_AFx_GPIO();
    GPIO_InitStruct.IT = GPIO_IT_NONE;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pins = GPIO_PIN_4;
    GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
    PA04_SETHIGH();    // 将PA4设置为高电平

    //=============================================
    // 配置UART2的TX/RX引脚：PB05/PB06
    GPIO_InitStruct.Pins = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Pins = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    PB05_AFx_UART2TXD();
    PB06_AFx_UART2RXD();
}

static void init_variables(void)
{

}
