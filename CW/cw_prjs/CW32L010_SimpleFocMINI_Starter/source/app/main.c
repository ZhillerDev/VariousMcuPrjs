#include "main.h"

/**
 * @brief Main function of project
 *
 * @return int
 */
int main(void)
{
    mcu_init();

    while (1)
    {
        iwdt_feed();

        // 10ms 周期任务
        if (u8_timer_10ms_flag)
        {
            u8_timer_10ms_flag = 0; // 清除标志位
                                    // TODO: 添加10ms周期的任务，如：
                                    // - 按键扫描
                                    // - LED刷新
                                    // - 数据采集
                                    // pwm_breath_test();
                                    // adc_convert_dispatch();

            GPIO_TogglePin(CW_GPIOA, GPIO_PIN_2); // 切换PA2状态，作为10ms周期任务的示例
            // delay_us(20); // 短暂延时，确保GPIO状态变化被观察到
            delay_us(15); // 使用NOP指令进行短暂延时，确保GPIO状态变化被观察到
            GPIO_TogglePin(CW_GPIOA, GPIO_PIN_2);
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
            // printf("ADC: PB02=%u, Temperature=%f, Vref=%f\r\n", u16_adc_pb02, f_adc_temperature, f_adc_vref);
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
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
#ifdef USE_FULL_ASSERT
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
