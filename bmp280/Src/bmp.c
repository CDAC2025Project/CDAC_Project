 #include "bmp.h"
#include <stdio.h>

// BMP280 initialization  
uint8_t BMP280_Init(void)
{
    UartPuts("BMP280 INIT START\r\n");

    // Read chip ID
    I2CStart();
    I2CSendSlaveAddr(BMP280_ADDR << 1);   // Write
    I2CSendData(BMP280_REG_ID);           // Register 0xD0
    I2CRepeatStart();
    I2CSendSlaveAddr((BMP280_ADDR << 1)|1); // Read
    uint8_t id = I2CRecvDataNAck();
    I2CStop();

    char buf[40];
    sprintf(buf, "Read BMP280 ID: 0x%02X\r\n", id);
    UartPuts(buf);

    if (id != 0x58)
    {
        UartPuts("BMP280 ID FAIL\r\n");
        return 1;  // fail
    }

    UartPuts("BMP280 ID OK\r\n");

    // Configure ctrl_meas register:
    // osrs_p = x1 (001), osrs_t = x1 (001), mode = normal (11)
    // => 0x27
    I2CStart();
    I2CSendSlaveAddr(BMP280_ADDR << 1);   // Write
    I2CSendData(BMP280_REG_CTRL_MEAS);    // Register 0xF4
    I2CSendData(0x27);                    // x1 oversampling, normal mode
    I2CStop();

    UartPuts("BMP280 CONFIG DONE\r\n");

    return 0;  // success
}

// Read RAW pressure (20-bit)
int32_t BMP280_ReadPressure(void)
{
    I2CStart();
    I2CSendSlaveAddr(BMP280_ADDR << 1);   // Write
    I2CSendData(BMP280_REG_PRESS_MSB);    // 0xF7
    I2CRepeatStart();
    I2CSendSlaveAddr((BMP280_ADDR << 1)|1); // Read

    uint8_t msb  = I2CRecvDataAck();
    uint8_t lsb  = I2CRecvDataAck();
    uint8_t xlsb = I2CRecvDataNAck();
    I2CStop();

    // Combine 20-bit value
    int32_t adc_P = ((int32_t)msb << 12) |
                    ((int32_t)lsb << 4)  |
                    ((xlsb >> 4) & 0x0F);

    return adc_P;  // RAW pressure
}
