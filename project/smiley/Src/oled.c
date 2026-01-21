 #include "oled.h"
#include "spi.h"
#include "uart.h"

/* Small delay functions */
static void oled_delay_short(void) {
    for (volatile int i = 0; i < 20000; i++);
}

static void oled_delay_long(void) {
    for (volatile int i = 0; i < 400000; i++);
}

/* Initialize PB0 (DC) and PB1 (RES) as outputs */
void OLED_GPIO_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* PB0=DC, PB1=RES -> output */
    GPIOB->MODER &= ~((3U << (0*2)) | (3U << (1*2)));
    GPIOB->MODER |= ((1U << (0*2)) | (1U << (1*2)));

    GPIOB->OTYPER &= ~((1U<<0)|(1U<<1));
    GPIOB->OSPEEDR |= (3U << (0*2)) | (3U << (1*2));
    GPIOB->PUPDR &= ~((3U << (0*2)) | (3U << (1*2)));

    OLED_DC_LOW();
    OLED_RES_HIGH();
}

/* Send command/data */
void OLED_SendCommand(uint8_t cmd) {
    OLED_DC_LOW();
    SpiCSEnable();
    SpiTransmit(cmd);
    SpiCSDisable();
}

void OLED_SendData(uint8_t data) {
    OLED_DC_HIGH();
    SpiCSEnable();
    SpiTransmit(data);
    SpiCSDisable();
}

/* Set page/column cursor */
void OLED_SetCursor(uint8_t page, uint8_t column) {
    OLED_SendCommand(0xB0 + page);
    OLED_SendCommand(column & 0x0F);
    OLED_SendCommand(0x10 | ((column >> 4) & 0x0F));
}

/* Initialize OLED */
void OLED_Init(void) {
    OLED_GPIO_Init();
    SpiInit();

    /* Reset OLED */
    OLED_RES_LOW();
    oled_delay_short();
    OLED_RES_HIGH();
    oled_delay_long();

    /* Init commands */
    OLED_SendCommand(0xAE);
    OLED_SendCommand(0xD5); OLED_SendCommand(0x80);
    OLED_SendCommand(0xA8); OLED_SendCommand(0x3F);
    OLED_SendCommand(0xD3); OLED_SendCommand(0x00);
    OLED_SendCommand(0x40);
    OLED_SendCommand(0x8D); OLED_SendCommand(0x14);
    OLED_SendCommand(0x20); OLED_SendCommand(0x00);
    OLED_SendCommand(0xA1);
    OLED_SendCommand(0xC8);
    OLED_SendCommand(0xDA); OLED_SendCommand(0x12);
    OLED_SendCommand(0x81); OLED_SendCommand(0xCF);
    OLED_SendCommand(0xD9); OLED_SendCommand(0xF1);
    OLED_SendCommand(0xDB); OLED_SendCommand(0x40);
    OLED_SendCommand(0xA4);
    OLED_SendCommand(0xA6);
    OLED_SendCommand(0xAF);
}

/* Clear screen */
void OLED_Clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        OLED_SetCursor(page,0);
        for (uint8_t col = 0; col < 128; col++)
            OLED_SendData(0x00);
    }
}

/* Display bitmap (128x64) */
void OLED_DisplayBitmap(const uint8_t *bitmap) {
    for (uint8_t page = 0; page < 8; page++) {
        OLED_SetCursor(page,0);
        for (uint8_t col = 0; col < 128; col++) {
            OLED_SendData(bitmap[page*128 + col]);
        }
    }
}
