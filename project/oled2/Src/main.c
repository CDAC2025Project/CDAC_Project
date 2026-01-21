 #include "stm32f4xx.h"
#include "uart.h"
#include "spi.h"
#include "oled.h"

/* tiny delay */
static void delay_ms_approx(uint32_t ms)
{
    for (volatile uint32_t j = 0; j < ms * 2000; j++);
}

int main(void)
{
    /* Initialize UART for debug */
    UartInit(115200);
    UartPuts("\r\n========================================\r\n");
    UartPuts("STM32F407 - SSD1306 SPI OLED - DEBUG\n");
    UartPuts("Author: Sagar\n");
    UartPuts("========================================\r\n");

    /* Basic CPU alive check */
    UartPuts("Main: Starting hardware init sequence...\r\n");

    /* Initialize SPI (SpiInit also prints message if coded) */
    UartPuts("Main: Initializing SPI (SpiInit)...\r\n");
    SpiInit();
    UartPuts("Main: SPI initialized.\r\n");

    /* Initialize OLED */
    UartPuts("Main: Calling OLED_Init()...\r\n");
    OLED_Init();
    UartPuts("Main: OLED_Init() complete.\r\n");

    /* Clear and print */
    UartPuts("Main: Clearing display...\r\n");
    OLED_Clear();

    /* Set cursor to page0, col0 and print */
    UartPuts("Main: Set cursor to page 0, col 0\n");
    OLED_SetCursor(0, 0);

    UartPuts("Main: Printing 'HELLO FROM STM32' on OLED...\r\n");
    OLED_PrintString("HELLO FROM STM32");

    UartPuts("Main: Display update done. Entering main loop.\r\n");
    UartPuts("========================================\r\n");

    while (1)
    {
        /* heartbeat log every ~2s */
        UartPuts("Main: Heartbeat - system running...\r\n");
        delay_ms_approx(2000);
    }
}
