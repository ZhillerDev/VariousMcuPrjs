#include "main.h"

void mcu_config(void)
{
    SYSCTRL_HSI_Enable(SYSCTRL_HSIOSC_DIV6);        // SYSCLK = 48MHZ / 6 = 8MHZ
    SYSCTRL_LSI_Enable();                           // 使能LSI时钟
    SYSCTRL_HCLKPRS_Config(SYSCTRL_HCLK_DIV1);      // HCLK = 8MHZ 不分频
    SYSCTRL_PCLKPRS_Config(SYSCTRL_PCLK_DIV1);      // PCLK = 8MHZ 不分频

    SYSCTRL_SystemCoreClockUpdate(8000000);         // 更新系统时钟
    SysTick_Config(SystemCoreClock / 1000);         // 配置SysTick为1ms中断周期，1khz    

    __SYSCTRL_IWDT_CLK_ENABLE();                    // 使能IWDT模块
    __SYSCTRL_BTIM123_CLK_ENABLE();                 // 使能BTIM1/2/3时钟
    __SYSCTRL_LPTIM_CLK_ENABLE();                   // 使能LPTIM时钟
    __SYSCTRL_GTIM1_CLK_ENABLE();                   // 使能GTIM1时钟
    __SYSCTRL_ADC_CLK_ENABLE();                 // 使能ADC时钟
    __SYSCTRL_GPIOB_CLK_ENABLE();                   // 使能GPIOB时钟
    __SYSCTRL_GPIOA_CLK_ENABLE();                   // 使能GPIOA时钟

    timer_base_config();

    iwdt_config();

    uart_config();

    adc_config();

    // pwm_config();

    // capture_config();
}

/**
 * @brief Main function of project
 *
 * @return int
 */
int main(void)
{
    mcu_config();
    
    // 初始化定时器（如果mcu_config中没有初始化的话）
    // timer_base_config();

    while (1)
    {
        iwdt_feed();

        // 10ms 周期任务
        if (u8_timer_10ms_flag)
        {
            u8_timer_10ms_flag = 0;  // 清除标志位
            // TODO: 添加10ms周期的任务，如：
            // - 按键扫描
            // - LED刷新
            // - 数据采集
            // pwm_breath_test();
            adc_convert_dispatch();
        }
        
        // 50ms 周期任务
        if (u8_timer_50ms_flag)
        {
            u8_timer_50ms_flag = 0;
            // TODO: 添加50ms周期的任务，如：
            // - 数码管显示刷新
            // - 传感器数据读取
        }
        
        // 100ms 周期任务
        if (u8_timer_100ms_flag)
        {
            u8_timer_100ms_flag = 0;
            // TODO: 添加100ms周期的任务，如：
            // - 系统状态更新
            // - 数据处理
            // printf("%u %u\r\n", u16_capture_freq, u16_capture_duty);
            printf("ADC: PB02=%u, Temperature=%f, Vref=%f\r\n", u16_adc_pb02, f_adc_temperature, f_adc_vref);
        }
        
        // 200ms 周期任务
        if (u8_timer_200ms_flag)
        {
            u8_timer_200ms_flag = 0;
            // TODO: 添加200ms周期的任务，如：
            // - LED闪烁控制
            // - 通信发送
            // PA02_TOG();
            // printf("200ms\r\n");
            // UART_SendString(CW_UART2, "123\r\n");
        }
        
        // 执行其他非周期任务或低优先级任务
        // TODO: 添加其他任务
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @return None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

