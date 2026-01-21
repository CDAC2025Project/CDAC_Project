 #include "stm32f4xx.h"
#include "i2c.h"
#include "uart.h"
#include "bmp.h"
#include <stdio.h>

void delay_ms(uint32_t ms){
    for(uint32_t i=0;i<ms*16000;i++);
}

int main(void) {
    uint32_t pressure;
    char buf[64];

    UartInit(115200);
    UartPuts("UART OK\r\n");

    I2CInit();
    UartPuts("I2C OK\r\n");

    UartPuts("BMP280 INIT START\r\n");
    if(BMP280_Init() == 0) {
        UartPuts("BMP280 INIT OK\r\n");
    } else {
        UartPuts("BMP280 INIT FAILED\r\n");
        while(1) {
            delay_ms(500);
        }
    }

    while(1) {
        int32_t raw = BMP280_ReadPressure();
        sprintf(buf, "Raw Pressure ADC: %ld\r\n", (long)raw);
        UartPuts(buf);
        delay_ms(500);
    }
}
