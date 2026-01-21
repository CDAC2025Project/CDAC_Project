 #include "stm32f4xx.h"
#include "uart.h"
#include "spi.h"
#include "oled.h"

/* Tiny delay */
static void delay_ms(uint32_t ms) {
    for (volatile uint32_t j=0; j<ms*2000; j++);
}

/* Simple smiley 64x64 bitmap (8 pages x 64 columns) */
const uint8_t smiley[512] = {
    // page0
    0x00,0x3C,0x42,0xA9,0x85,0x85,0xA9,0x91,0x42,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x3C,0x42,0xA5,0x81,0x81,0xA5,0x91,0x42,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,
    // page1..7 -> repeat zeros for simplicity
    [32 ... 511] = 0x00
};

int main(void) {
    UartInit(115200);
    UartPuts("STM32F407 - OLED Smiley Demo\r\n");

    SpiInit();
    OLED_Init();
    OLED_Clear();

    /* Display Smiley */
    OLED_DisplayBitmap(smiley);

    while(1) {
        // just loop
    }
}
