#include "oled.h"
#include "font6x8.h"

/* small delay used for reset/timing */
static void oled_delay_short(void)
{
	for (volatile int i = 0; i < 20000; i++);

	/* ~ short */
}
static void oled_delay_long(void)
{
	for (volatile int i = 0; i < 400000; i++);
/* ~ longer */
}


/* Initialize PB0 (DC) and PB1 (RES) as outputs */
void OLED_GPIO_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* PB0 and PB1 -> General purpose output (01) */
    GPIOB->MODER &= ~((3U << (0 * 2)) | (3U << (1 * 2)));
    GPIOB->MODER |=  ((1U << (0 * 2)) | (1U << (1 * 2)));

    /* push-pull */
    GPIOB->OTYPER &= ~((1U << 0) | (1U << 1));

    /* high speed */
    GPIOB->OSPEEDR |= (3U << (0 * 2)) | (3U << (1 * 2));

    /* no pull-up/pull-down */
    GPIOB->PUPDR &= ~((3U << (0 * 2)) | (3U << (1 * 2)));

    OLED_DC_LOW();
    OLED_RES_HIGH();
}

/* Send single command byte over SPI */
void OLED_SendCommand(uint8_t cmd)
{
    OLED_DC_LOW();          /* command mode */
    SpiCSEnable();
    SpiTransmit(cmd);
    SpiCSDisable();
}

/* Send single data byte over SPI */
void OLED_SendData(uint8_t data)
{
    OLED_DC_HIGH();         /* data mode */
    SpiCSEnable();
    SpiTransmit(data);
    SpiCSDisable();
}

/* Set page and column cursor (0..7 pages, 0..127 columns) */
void OLED_SetCursor(uint8_t page, uint8_t column)
{
    OLED_SendCommand(0xB0 + (page & 0x07));          // set page
    OLED_SendCommand(column & 0x0F);                 // set low column
    OLED_SendCommand(0x10 | ((column >> 4) & 0x0F)); // set high column
}

/* Initialize SSD1306 via SPI */
void OLED_Init(void)
{
    /* init GPIOs for DC/RES */
    OLED_GPIO_Init();

    /* SPI must be initialized here or by caller */
    SpiInit();

    /* Reset pulse */
    OLED_RES_LOW();
    oled_delay_short();
    OLED_RES_HIGH();
    oled_delay_long();

    /* Initialization command sequence */
    OLED_SendCommand(0xAE);             // Display OFF
    OLED_SendCommand(0xD5); OLED_SendCommand(0x80); // Clock divide
    OLED_SendCommand(0xA8); OLED_SendCommand(0x3F); // Multiplex ratio
    OLED_SendCommand(0xD3); OLED_SendCommand(0x00); // Display offset
    OLED_SendCommand(0x40);             // Start line = 0
    OLED_SendCommand(0x8D); OLED_SendCommand(0x14); // Charge pump ON
    OLED_SendCommand(0x20); OLED_SendCommand(0x00); // Horizontal addressing
    OLED_SendCommand(0xA1);             // Segment remap
    OLED_SendCommand(0xC8);             // COM scan direction
    OLED_SendCommand(0xDA); OLED_SendCommand(0x12); // COM config
    OLED_SendCommand(0x81); OLED_SendCommand(0xCF); // Contrast
    OLED_SendCommand(0xD9); OLED_SendCommand(0xF1); // Pre-charge
    OLED_SendCommand(0xDB); OLED_SendCommand(0x40); // VCOMH
    OLED_SendCommand(0xA4);             // Resume display
    OLED_SendCommand(0xA6);             // Normal display
    OLED_SendCommand(0xAF);             // Display ON

    oled_delay_short();
}

/* Clear full display RAM */
void OLED_Clear(void)
{
    for (uint8_t page = 0; page < 8; page++)
    {
        OLED_SetCursor(page, 0);
        for (uint8_t col = 0; col < 128; col++)
        {
            OLED_SendData(0x00);
        }
    }
}

/* Print one 6x8 char */
void OLED_PrintChar(char c)
{
    if (c < 32 || c > 127) c = '?';
    uint8_t idx = (uint8_t)(c - 32);

    for (int i = 0; i < 6; i++)
    {
        OLED_SendData(font6x8[idx][i]);
    }
}

/* Print ASCII string (no wrapping management) */
void OLED_PrintString(const char *str)
{
    while (*str)
    {
        OLED_PrintChar(*str++);
    }
}
