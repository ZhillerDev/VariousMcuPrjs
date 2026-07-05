#include "uart.h"

void uart_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    UART_InitTypeDef UART_InitStructure;

    // 使能GPIOB和UART2时钟
    SYSCTRL_AHBPeriphClk_Enable(SYSCTRL_AHB_PERIPH_GPIOB, ENABLE);
    SYSCTRL_APBPeriphClk_Enable1(SYSCTRL_APB1_PERIPH_UART2, ENABLE);

    // 配置TXD和RXD引脚
    GPIO_InitStructure.Pins = GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.Pins = GPIO_PIN_6;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_Init(CW_GPIOB, &GPIO_InitStructure);
    PB05_AFx_UART2TXD();
    PB06_AFx_UART2RXD();

    // 配置UART2
    UART_InitStructure.UART_BaudRate = 115200;
    UART_InitStructure.UART_Over = UART_Over_16;
    UART_InitStructure.UART_Source = UART_Source_PCLK;
    UART_InitStructure.UART_UclkFreq = 8000000;
    UART_InitStructure.UART_StartBit = UART_StartBit_FE;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No ;
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
    UART_Init(CW_UART2, &UART_InitStructure);
    UART_ITConfig(CW_UART2, UART_IT_RC, ENABLE);

    __disable_irq();
    //UARTx中断使能
    NVIC_SetPriority(UART2_IRQn, 0);
    NVIC_EnableIRQ(UART2_IRQn);
    __enable_irq();
}

void UART2_IRQHandler(void)
{
    /* USER CODE BEGIN */
    uint8_t TxRxBuffer;
    if (UART_GetITStatus(CW_UART2, UART_IT_RC) != RESET)
    {
        TxRxBuffer = UART_ReceiveData_8bit(CW_UART2);
        UART_SendData_8bit(CW_UART2, TxRxBuffer);
        UART_ClearITPendingBit(CW_UART2, UART_IT_RC);
    }
    /* USER CODE END */
}

int fputc(int ch, FILE *f)
{
    UART_SendData_8bit(CW_UART2, (uint8_t)ch);

    while (UART_GetFlagStatus(CW_UART2, UART_FLAG_TXE) == RESET);

    return ch;
}
