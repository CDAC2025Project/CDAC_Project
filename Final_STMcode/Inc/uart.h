/*
 * uart.h
 *
 *  Created on: Nov 6, 2025
 *      Author: sunbeam
 */
#include "stm32f4xx.h"
#ifndef UART_H_
#define UART_H_
#define BRR_9600    0x0683
#define BRR_38400  0x01A1
#define BRR_115200 0x008B


//PA2->RX ...PA3->TX
void UART2_Init(void);
void UART2_SendChar(char c);
void UART2_SendString(char *str);
void UART_Init(void);
void UART_SendString(char *str);

#endif /* UART_H_ */
