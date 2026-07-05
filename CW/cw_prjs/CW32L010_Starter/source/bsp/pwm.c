#include "pwm.h"

volatile uint16_t u16_capture_hightime = 0;   // 高电平时间
volatile uint16_t u16_capture_period = 0;     // 周期    
volatile uint16_t u16_capture_duty = 0;     // 占空比
volatile uint16_t u16_capture_freq = 0;     // 捕获频率

void pwm_config(void)
{
    /* GTIM1 配置PWM输出 */
    GTIM_InitTypeDef GTIM_InitStruct = {0};                             // 定时器时基结构体
    GTIM_OCModeCfgTypeDef GTIM_OCModeCfgStruct = {DISABLE, DISABLE, 0}; // 输出比较结构体

    __SYSCTRL_GTIM1_CLK_ENABLE(); // 使能GTIM1时钟

    // 配置定时器时基参数
    GTIM_InitStruct.AlignMode = GTIM_ALIGN_MODE_EDGE; // 边沿对齐模式
    GTIM_InitStruct.ARRBuffState = GTIM_ARR_BUFF_EN;  // 使能自动重载缓冲
    GTIM_InitStruct.Direction = GTIM_DIRECTION_UP;    // 向上计数
    GTIM_InitStruct.EventOption = GTIM_EVENT_NORMAL;  // 普通事件模式
    GTIM_InitStruct.Prescaler = 8 - 1;                // 预分频8 = 1MHz计数频率 (HCLK时钟为8MHz)
    GTIM_InitStruct.PulseMode = GTIM_PULSE_MODE_DIS;  // 禁用脉冲模式
    GTIM_InitStruct.ReloadValue = 1000 - 1;           // 自动重载值 = 1000 = 1ms周期
    GTIM_InitStruct.UpdateOption = GTIM_UPDATE_DIS;   // 禁用更新事件
    GTIM_TimeBaseInit(CW_GTIM1, &GTIM_InitStruct);    // 初始化定时器时基

    // 配置输出比较通道3
    GTIM_OCModeCfgStruct.FastMode = DISABLE;                   // 禁用快速模式
    GTIM_OCModeCfgStruct.OCMode = GTIM_OC_MODE_PWM1;           // PWM模式1
    GTIM_OCModeCfgStruct.OCPolarity = GTIM_OC_POLAR_NONINVERT; // 极性不反转
    GTIM_OCModeCfgStruct.PreloadState = DISABLE;               // 禁用预装载
    GTIM_OC3ModeCfg(CW_GTIM1, &GTIM_OCModeCfgStruct);          // 初始化OC3

    GTIM_SetCompare3(CW_GTIM1, 500 - 1); // 设置占空比比较值 = 500 (50%)
    GTIM_OC3Cmd(CW_GTIM1, ENABLE);       // 使能OC3输出
    GTIM_Cmd(CW_GTIM1, ENABLE);          // 使能GTIM1计数器

    // 配置中断
    __disable_irq();            // 关中断
    NVIC_EnableIRQ(GTIM1_IRQn); // 使能GTIM1中断
    __enable_irq();             // 开中断

    /* PB4 作为GTIM1的CH3 PWM 输出 */
    __SYSCTRL_GPIOB_CLK_ENABLE(); // 使能GPIOB时钟

    PB04_DIGTAL_ENABLE();   // PB4数字功能使能
    PB04_DIR_OUTPUT();      // PB4设为输出方向
    PB04_PUSHPULL_ENABLE(); // PB4推挽输出
    PB04_AFx_GTIM1CH3();    // PB4复用为GTIM1的CH3
}

void pwm_breath_test(void)
{
    static uint16_t duty = 0;   // 当前占空比 0~100
    static uint8_t dir = 1;     // 方向: 1增 0减
    
    dir ? duty++ : duty--;      // 根据方向增减
    
    if (duty >= 100) { duty = 100; dir = 0; }   // 到顶反转
    if (duty == 0) { dir = 1; }                 // 到底反转
    
    GTIM_SetCompare3(CW_GTIM1, duty * 10);      // 设置比较值(占空比=duty*10/1000)
}

void capture_config(void)
{
    GTIM_InitTypeDef GTIM_InitStruct = {0};
    GTIM_ICModeCfgTypeDef GTIM_ICModeCfgStruct = {0};
    LPTIM_InitTypeDef LPTIM_InitStruct = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* LPTIM1 配置PWM输出 */
    __SYSCTRL_LPTIM_CLK_ENABLE();
    LPTIM_InitStruct.LPTIM_ClockSource = LPTIM_CLOCK_SOURCE_MCLK;
    LPTIM_InitStruct.LPTIM_CounterMode = LPTIM_COUNTER_MODE_TIME;
    LPTIM_InitStruct.LPTIM_Prescaler = LPTIM_PRS_DIV8;
    LPTIM_InitStruct.LPTIM_Period = 1000 - 1;

    LPTIM_Init(&LPTIM_InitStruct);

    LPTIM_InternalClockConfig(LPTIM_ICLK_PCLK);

    CW_LPTIM->ICR = 0x00;
    LPTIM_PWMStart(1000 - 1, 500 - 1, LPTIM_PWM_POL_POS);

    /* PA05 配置LPTIM1输出 */ 
    GPIO_InitStruct.IT = GPIO_IT_NONE;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pins = GPIO_PIN_5;
    GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
    PA05_AFx_LPTIMOUT();

    /* GTIM1 配置捕获输入 */
    __SYSCTRL_GTIM1_CLK_ENABLE(); // 使能GTIM1时钟
    GTIM_InitStruct.AlignMode = GTIM_ALIGN_MODE_EDGE;         //边沿对齐
    GTIM_InitStruct.ARRBuffState = GTIM_ARR_BUFF_EN;          //使能缓存寄存器
    GTIM_InitStruct.Direction = GTIM_DIRECTION_UP;            //向上计数
    GTIM_InitStruct.EventOption = GTIM_EVENT_NORMAL;
    GTIM_InitStruct.Prescaler = 0;                            // 1分频 = 8MHz计数频率
    GTIM_InitStruct.PulseMode = GTIM_PULSE_MODE_DIS;
    GTIM_InitStruct.ReloadValue = 0xFFFF;                     // 自动重载值 = 0xFFFF = 65535
    GTIM_InitStruct.UpdateOption = GTIM_UPDATE_EN;        
    GTIM_TimeBaseInit(CW_GTIM1, &GTIM_InitStruct);

    GTIM_ICModeCfgStruct.ICMap = GTIM_IC3_MAP_TI3;                //IC1 映射到 TI1 上
    GTIM_ICModeCfgStruct.ICPrescaler = GTIM_IC_PRESCALER_DIV1;
    GTIM_ICModeCfgStruct.ICFilter = GTIM_IC_FILTER_PCLK_NONE ;
    GTIM_ICModeCfgStruct.ICPolarity = GTIM_ICPolarity_BothEdge ;    //双边沿捕获
    GTIM_IC3ModeCfg(CW_GTIM1, &GTIM_ICModeCfgStruct);
    GTIM_IC3Cmd(CW_GTIM1, ENABLE);
    GTIM_ITConfig(CW_GTIM1, GTIM_IT_CC3, ENABLE);	
    GTIM_Cmd(CW_GTIM1, ENABLE);

    GPIO_InitStruct.IT = GPIO_IT_NONE;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pins = GPIO_PIN_4;
    GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
    PA04_AFx_GTIM1CH3();

    // 配置中断
    __disable_irq();            // 关中断
    NVIC_EnableIRQ(GTIM1_IRQn); // 使能GTIM1中断
    __enable_irq();             // 开中断
}

void capture_calculate(void)
{
    if (u16_capture_period > 0)
    {
        u16_capture_freq = 8000000 / u16_capture_period;   // 假设GTIM时钟=8MHz，频率(Hz)
        u16_capture_duty = (u16_capture_hightime * 100) / u16_capture_period;  // 占空比(%)
    }
}

void GTIM1_IRQHandler(void)
{
    static uint8_t u8_capture_step = 0;

    /* USER CODE BEGIN */
    if (GTIM_GetITStatus(CW_GTIM1, GTIM_IT_TI))
    {
        GTIM_ClearITPendingBit(CW_GTIM1, GTIM_IT_TI);
    }

    /* 捕获中断 */
    if (GTIM_GetITStatus(CW_GTIM1, GTIM_IT_CC3))
    {
        GTIM_ClearITPendingBit(CW_GTIM1, GTIM_IT_CC3);

        uint16_t u16_current_cnt = GTIM_GetCapture3(CW_GTIM1);
        uint8_t pin_level = PA04_GETVALUE(); // 读取当前引脚电平 (判断是上升沿还是下降沿)
        
        switch (u8_capture_step)
        {
        case 0:  // 等待高电平开始 (上升沿)
            GTIM_SetCounterValue(CW_GTIM1, 0); // 捕获开始，计数器清零
            if (pin_level == 1)  // 上升沿
            {
                u8_capture_step = 1;
            }
            break;
        case 1:  // 下降沿，计算高电平时间
            u16_capture_hightime = u16_current_cnt;
            u8_capture_step = 2;
            break;
        case 2:  // 下一个上升沿，计算周期
            u16_capture_period = u16_current_cnt;
            capture_calculate();  // 立即计算频率和占空比
            // 准备下一次测量
            u8_capture_step = 0;
            break;
        default:
            break;
        }
    }
    
    /* USER CODE END */
}

/**
 * @brief 呼吸灯测试函数，每隔10ms调用一次，实现占空比0→100→0循环
 * @note  前提：需先调用pwm_config()初始化PWM，周期设为1000
 *        调用位置：在10ms定时器中断中调用
 *        效果：占空比每10ms变化1%，单程1秒，完整周期2秒
 */
