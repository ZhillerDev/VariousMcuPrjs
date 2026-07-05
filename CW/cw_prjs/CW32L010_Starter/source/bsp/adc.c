#include "adc.h"

uint16_t g_adc_value[ADC_TOTAL_CHANNEL * 3] = {0}; // 存储ADC采样临时计算结果的全局变量
float f_adc_vref = 0;                         // 存储ADC参考电压的变量
float f_adc_temperature = 0;                       // 存储ADC温度传感器的变量
uint16_t u16_adc_pb02 = 0;                         // 存储PB02的变量

void adc_config(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};

    PB02_ANALOG_ENABLE(); // 使能PB02模拟输入功能

    __SYSCTRL_ADC_CLK_ENABLE();

    ADC_InitStructure.ADC_ClkDiv = ADC_Clk_Div8;                    // ADC时钟不分频
    ADC_InitStructure.ADC_ConvertMode = ADC_ConvertMode_Continuous; // 连续采样
    ADC_InitStructure.ADC_SQREns = ADC_SqrEns0to2;                  // 三个通道
    ADC_InitStructure.ADC_IN0.ADC_InputChannel = ADC_InputCH9;
    ADC_InitStructure.ADC_IN1.ADC_InputChannel = ADC_InputTs;
    ADC_InitStructure.ADC_IN2.ADC_InputChannel = ADC_InputVref1P2;
    ADC_InitStructure.ADC_IN0.ADC_SampTime = ADC_SampTime54Clk;
    ADC_InitStructure.ADC_IN1.ADC_SampTime = ADC_SampTime54Clk;
    ADC_InitStructure.ADC_IN2.ADC_SampTime = ADC_SampTime390Clk;
    ADC_Init(&ADC_InitStructure);
    ADC_ClearITPendingAll();

    ADC_SetTs(ADC_TsEnable); // 使能温度传感器，使能约30μs后才可以转换TS通道
    SysTickDelay(1);         // 在此延时1ms
	CW_ADC->CR_f.BGREN = 1;//使能BGR1.2V ，内部BGR的启动时间大约为30μs
	SysTickDelay(1);//在此延时1ms

    // ADC使能
    ADC_Enable();
    ADC_SoftwareStartConvCmd(ENABLE);
}

void adc_convert_dispatch(void)
{
    uint8_t i;
    static uint8_t calc_cnt = 0;

    calc_cnt++;

    if (calc_cnt >= ADC_FILTER_COUNT)
    {
        calc_cnt = 0;
        // 第5次调用，计算最终结果
        u16_adc_pb02 = adc_average_calc(0, 1);
        f_adc_temperature = ADC_GetTs(3.3f, adc_average_calc(1, 1));
        f_adc_vref = adc_vdd_calc(ADC_GetConversionValue(2));
    }
    else
    {
        // 前4次调用，只累加数据
        for (i = 0; i < ADC_TOTAL_CHANNEL; i++)
        {
            adc_average_calc(i, 0);
        }
    }
}

static uint16_t adc_average_calc(uint8_t channel, uint8_t is_result)
{
    uint16_t current_value = ADC_GetConversionValue(channel);
    uint8_t sum_idx = channel;                         // 累加和索引
    uint8_t max_idx = channel + ADC_TOTAL_CHANNEL;     // 最大值索引
    uint8_t min_idx = channel + ADC_TOTAL_CHANNEL * 2; // 最小值索引
    uint16_t result = 0;

    g_adc_value[sum_idx] += current_value; // 累加当前值到总和中

    if (is_result)
    {
        
        // 计算平均值（去掉最大最小值）
        uint32_t sum = g_adc_value[sum_idx];
        uint16_t max_val = g_adc_value[max_idx];
        uint16_t min_val = g_adc_value[min_idx];

        // (总和 - 最大值 - 最小值) / (ADC_FILTER_COUNT - 2)
        if (ADC_FILTER_COUNT > 2)
        {
            result = (uint16_t)((sum - max_val - min_val) / (ADC_FILTER_COUNT - 2));
        }
        else
        {
            result = (uint16_t)(sum / ADC_FILTER_COUNT);
        }

        // 重置该通道的滤波缓冲区
        g_adc_value[sum_idx] = 0;
        g_adc_value[max_idx] = 0;
        g_adc_value[min_idx] = 0xFFFF; // 最小值重置为最大值
    }
    else
    {
        // 更新最大值
        if (current_value > g_adc_value[max_idx])
        {
            g_adc_value[max_idx] = current_value;
        }

        // 更新最小值
        if (current_value < g_adc_value[min_idx])
        {
            g_adc_value[min_idx] = current_value;
        }
    }

    return result;
}

static uint16_t adc_voltage_calc(uint16_t adc_value)
{
    return (adc_value * 3300) / 4095; // 假设ADC分辨率为12位，参考电压为3.3V
}

static float adc_vdd_calc(uint16_t adc_value)
{
    return (*(uint16_t *)0x001007D2) * 4.095f / adc_value; // 计算VDD电压，假设内部参考电压为1.2V
}