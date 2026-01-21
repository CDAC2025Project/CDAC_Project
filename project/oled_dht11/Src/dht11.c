 #include "oled.h"
#include "dht11.h"
#include "uart.h"
#include "delay.h"
#include <stdio.h>

int main(void)
{
    UART_Init();
    OLED_Init();
    OLED_Clear();
    DHT11_Init();

    UART_Puts("System Initialized.\r\n");
    OLED_SetCursor(0,0);
    OLED_PrintString("Sagar");
    UART_Puts("OLED: Printed name Sagar.\r\n");

    uint8_t temp, hum;
    char buf[30];

    while(1)
    {
        if(DHT11_Read(&temp, &hum))
        {
            sprintf(buf, "Temp: %dC", temp);
            OLED_SetCursor(2,0);
            OLED_PrintString(buf);

            sprintf(buf, "Hum: %d%%", hum);
            OLED_SetCursor(3,0);
            OLED_PrintString(buf);

            sprintf(buf, "DEBUG: Temp=%d C, Hum=%d %%\r\n", temp, hum);
            UART_Puts(buf);
        }
        else
        {
            UART_Puts("ERROR: DHT11 read failed!\r\n");
        }

        delay_ms(1000);
    }
}
