#ifndef __ADC_H__
#define __ADC_H__

#include "global_config.h"

#define ADC_TOTAL_CHANNEL 3
#define ADC_FILTER_COUNT 5

extern uint16_t g_adc_value[ADC_TOTAL_CHANNEL * 3]; // 存储ADC采样临时计算结果的全局变量
extern float f_adc_vref;            // 存储ADC参考电压的变量
extern float f_adc_temperature;     // 存储ADC温度传感器的变量
extern uint16_t u16_adc_pb02;            // 存储PB02的变量

void adc_config(void);
void adc_convert_dispatch(void);

static uint16_t adc_average_calc(uint8_t channel, uint8_t is_result);
static uint16_t adc_voltage_calc(uint16_t adc_value);
static float adc_vdd_calc(uint16_t adc_value);

#endif
