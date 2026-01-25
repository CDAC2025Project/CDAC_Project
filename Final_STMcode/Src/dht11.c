/*
 * dht11.c
 *
 *  Created on: Nov 6, 2025
 *      Author: sunbeam
 */
#include "dht11.h"

void DHT11_Init(void)
{
    RCC->AHB1ENR |= (1 << 0); // GPIOA clock enable
}

static void DHT11_SetPinOutput(void)
{
    DHT11_PORT->MODER &= ~(3 << (DHT11_PIN * 2));
    DHT11_PORT->MODER |=  (1 << (DHT11_PIN * 2)); // output
}

static void DHT11_SetPinInput(void)
{
    DHT11_PORT->MODER &= ~(3 << (DHT11_PIN * 2)); // input
}

void DHT11_Start(void)
{
    DHT11_SetPinOutput();
    DHT11_PORT->BSRR = (1 << (DHT11_PIN + 16)); // low

    delay_us(20);
    DHT11_PORT->BSRR = (1 << DHT11_PIN);        // high
    delay_us(30);
    DHT11_SetPinInput();
}

uint8_t DHT11_CheckResponse(void)
{
    uint8_t response = 0;
    delay_us(40);
    if (!(DHT11_PORT->IDR & (1 << DHT11_PIN)))
    {
        delay_us(80);
        if (DHT11_PORT->IDR & (1 << DHT11_PIN)) response = 1;
        delay_us(80);
    }
    return response;
}

uint8_t DHT11_ReadByte(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        while (!(DHT11_PORT->IDR & (1 << DHT11_PIN)));
        delay_us(40);
        if (!(DHT11_PORT->IDR & (1 << DHT11_PIN)))
            i &= ~(1 << (7 - j));
        else
        {
            i |= (1 << (7 - j));
            while (DHT11_PORT->IDR & (1 << DHT11_PIN));
        }
    }
    return i;
}

uint8_t DHT11_ReadData(uint8_t *temp, uint8_t *humidity)
{
    uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2, SUM;
    DHT11_Start();
    if (DHT11_CheckResponse())
    {
        Rh_byte1 = DHT11_ReadByte();
        Rh_byte2 = DHT11_ReadByte();
        Temp_byte1 = DHT11_ReadByte();
        Temp_byte2 = DHT11_ReadByte();
        SUM = DHT11_ReadByte();

        if (SUM == (Rh_byte1 + Rh_byte2 + Temp_byte1 + Temp_byte2))
        {
            *humidity = Rh_byte1;
            *temp = Temp_byte1;
            return 1; // success
        }
    }
    return 0; // fail
}


