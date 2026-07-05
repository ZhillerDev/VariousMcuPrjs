#include "uart.h"

void uart_config(void)
{
    UART_InitTypeDef UART_InitStructure;

    // ≈‰÷√UART2
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
    //UARTx÷–∂œ πƒ‹
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
