 #include "stm32f4xx.h"
#include "uart.h"
#include "spi.h"
#include "oled.h"

// Simple delay function (blocking)
void delay_ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms * 4000; i++);
}

int main(void)
{
    // --- Initialize UART ---
    UartInit(115200);
    UartPuts("DEBUG: UART Initialized\r\n");

    // Small delay to stabilize peripherals
    delay_ms(100);

    // --- Initialize SPI ---
    SpiInit();
    UartPuts("DEBUG: SPI Initialized\r\n");

    delay_ms(100);

    // --- Initialize OLED ---
    OLED_Init();
    UartPuts("DEBUG: OLED Initialized\r\n");

    delay_ms(100);

    // --- Clear display ---
    OLED_Clear();
    UartPuts("DEBUG: OLED Screen Cleared\r\n");

    // --- Print string ---
    OLED_PrintString("HELLO SAGAR!");
    UartPuts("DEBUG: OLED Display Updated with message\r\n");

    // --- Main loop ---
    while(1)
    {
        // Optional: you can toggle an LED or send periodic debug messages
        // UartPuts("DEBUG: MCU Running...\r\n");
        delay_ms(1000);
    }

    return 0; // never reached
}
