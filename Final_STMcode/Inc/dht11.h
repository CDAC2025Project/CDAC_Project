/*
 * dht11.h
 *
 *  Created on: Nov 6, 2025
 *      Author: sunbeam
 */
#include "stm32f4xx.h"

#ifndef DHT11_H_
#define DHT11_H_

#define DHT11_PORT GPIOA
#define DHT11_PIN  1

void DHT11_Init(void);
void DHT11_Start(void);
uint8_t DHT11_CheckResponse(void);
uint8_t DHT11_ReadByte(void);
uint8_t DHT11_ReadData(uint8_t *temp, uint8_t *humidity);

#endif /* DHT11_H_ */
