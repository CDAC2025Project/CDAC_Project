/*
 * delay.c
 *
 *  Created on: Nov 28, 2025
 *      Author: sunbeam
 */


#include "delay.h"

void delay_us(uint32_t us)
{
    volatile uint32_t count;
    while(us--)
        for(count=0; count<16; count++); // approximate 1us for 168 MHz
}

void delay_ms(uint32_t ms)
{
    while(ms--) delay_us(1000);
}
