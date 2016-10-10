#include "mInclude.h"

/**************************timing***************************/
__IO uint32_t SysTime;
__IO uint8_t ErrorCode = 0;

void HAL_SYSTICK_Callback(void)
{
    SysTime++;
}

uint32_t millis(void)
{
#ifdef USE_MICROS
    return (uint32_t)(SysTime / 100);
#else
    return SysTime;
#endif
}

#ifdef USE_MICROS
void delay(uint32_t _delay)
{
    HAL_Delay(_delay * 100);
}

uint32_t micros(void)
{
    return (uint32_t)(SysTime * 10);
}
#endif

extern UART_HandleTypeDef huart1;

/**************************print***************************/
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    huart1.Instance->DR = (uint8_t) ch;
    /* Loop until the end of transmission */
    while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET);

    return ch;
}

