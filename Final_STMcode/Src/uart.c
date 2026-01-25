/*
 * uart.c
 *
 *  Created on: Sep 18, 2025
 *  Modified for FreeRTOS UART task usage
 */

#include "uart.h"

/* ===================== Low-level Init ===================== */
void UartInit(uint32_t baud)
{
    /* Enable GPIOA clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* PA2 (TX), PA3 (RX) → Alternate function */
    GPIOA->MODER &= ~(BV(4) | BV(6));
    GPIOA->MODER |=  (BV(5) | BV(7));

    /* No pull-up / pull-down */
    GPIOA->PUPDR &= ~(BV(4)|BV(5)|BV(6)|BV(7));

    /* AF7 for USART2 */
    GPIOA->AFR[0] &= ~(BV(15) | BV(11));
    GPIOA->AFR[0] |=  (BV(14)|BV(13)|BV(12)|BV(10)|BV(9)|BV(8));

    /* Enable USART2 clock */
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* Enable USART */
    USART2->CR1 = USART_CR1_UE;

    /* Baud rate */
    if (baud == 9600)
        USART2->BRR =BRR_9600;
    else if (baud == 38400)
        USART2->BRR =BRR_38400;
    else if (baud == 115200)
        USART2->BRR =BRR_115200;

    /* Enable TX & RX */
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
}

/* ===================== Character TX ===================== */
void UartPutch(int ch)
{
    while ((USART2->SR & USART_SR_TXE) == 0)
        ;
    USART2->DR = ch;
}

/* ===================== String TX ===================== */
void UartPuts(char str[])
{
    int i;
    for (i = 0; str[i] != '\0'; i++)
        UartPutch(str[i]);
}

/* ===================== RX ===================== */
int UartGetch(void)
{
    while ((USART2->SR & USART_SR_RXNE) == 0)
        ;
    return (int)USART2->DR;
}

void UartGets(char str[])
{
    int i = 0;
    char ch;
    do
    {
        ch = UartGetch();
        str[i++] = ch;
    } while (ch != '\r');

    str[i++] = '\n';
    str[i] = '\0';
}

/* ========================================================= */
/* ======= RTOS-FRIENDLY WRAPPER FUNCTIONS ================= */
/* ========================================================= */

/* Called from UART task */
void UART_Init(void)
{
    UartInit(115200);   /* MUST match ESP32 baud rate */
}

/* Used by UART task */
void UART_SendString(char *str)
{
    UartPuts(str);
}
