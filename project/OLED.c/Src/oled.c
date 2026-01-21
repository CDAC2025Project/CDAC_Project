 #include "oled.h"

// ---------------- GPIO Init ----------------
void OLED_GPIO_Init(void)
{
    // Enable clock for PORTB
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // PB0 = DC, PB1 = RES -> Output mode
    GPIOB->MODER &= ~((3<<(0*2)) | (3<<(1*2)));
    GPIOB->MODER |=  ((1<<(0*2)) | (1<<(1*2)));

    GPIOB->OTYPER &= ~((1<<0) | (1<<1));
    GPIOB->OSPEEDR |= (3<<(0*2)) | (3<<(1*2)); // High speed
    GPIOB->PUPDR &= ~((3<<(0*2)) | (3<<(1*2)));

    OLED_DC_LOW();
    OLED_RES_HIGH();
}

// ---------------- Send Command ----------------
void OLED_SendCommand(uint8_t cmd)
{
    OLED_DC_LOW();
    SpiCSEnable();
    SpiTransmit(cmd);
    SpiCSDisable();
}

// ---------------- Send Data ----------------
void OLED_SendData(uint8_t data)
{
    OLED_DC_HIGH();
    SpiCSEnable();
    SpiTransmit(data);
    SpiCSDisable();
}

// ---------------- Cursor Function ----------------
void OLED_SetCursor(uint8_t page, uint8_t column)
{
    OLED_SendCommand(0xB0 + page);
    OLED_SendCommand(column & 0x0F);
    OLED_SendCommand(0x10 | (column >> 4));
}

// ---------------- OLED Init ----------------
void OLED_Init(void)
{
    UartPuts("OLED: Starting Init...\r\n");

    OLED_GPIO_Init();
    SpiInit();

    // Reset OLED
    UartPuts("OLED: Resetting...\r\n");
    OLED_RES_LOW();
    for(volatile int i=0;i<20000;i++);
    OLED_RES_HIGH();

    UartPuts("OLED: Sending init commands...\r\n");

    OLED_SendCommand(0xAE); // Display OFF
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

    UartPuts("OLED: Init Done!\r\n");
}

// ---------------- Clear Screen ----------------
void OLED_Clear(void)
{
    UartPuts("OLED: Clearing screen...\r\n");

    for(uint8_t page=0; page<8; page++)
    {
        OLED_SetCursor(page,0);
        for(uint8_t i=0;i<128;i++)
            OLED_SendData(0x00);
    }
}

// ---------------- Print Character ----------------
void OLED_PrintChar(char c)
{
    if(c < 32 || c > 127) return;
    uint16_t index = (c - 32) * 6;

    for(int i = 0; i < 6; i++)
        OLED_SendData(font6x8[index + i]);
}

// ---------------- Print Full String ----------------
void OLED_PrintString(char *str)
{
    while(*str)
        OLED_PrintChar(*str++);
}
