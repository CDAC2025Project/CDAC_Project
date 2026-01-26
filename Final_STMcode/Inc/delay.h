/*
 * delay.h
 *
 *  Created on: Nov 6, 2025
 *      Author: sunbeam
 */
#include "stm32f4xx.h"

#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx.h"   // change if different MCU

void DWT_Delay_Init(void);
void delay_us(uint32_t us);

#endif

