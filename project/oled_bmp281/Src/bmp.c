 #include "bmp.h"
#include <stdio.h>

BMP280_Calib bmp280_calib;
int32_t t_fine;

// Helper: read 16-bit unsigned register
static uint16_t read16(uint8_t reg) {
    I2CStart();
    I2CSendSlaveAddr(BMP280_ADDR << 1);  // write
    I2CSendData(reg);
    I2CRepeatStart();
    I2CSendSlaveAddr((BMP280_ADDR << 1) | 1); // read
    uint8_t msb = I2CRecvDataAck();
    uint8_t lsb = I2CRecvDataNAck();
    I2CStop();
    return (msb << 8) | lsb;
}

// Helper: read signed 16-bit register
static int16_t readS16(uint8_t reg) {
    return (int16_t)read16(reg);
}

// BMP280 initialization
uint8_t BMP280_Init(void) {
    UartPuts("BMP280 INIT START\r\n");

    // Read chip ID
    I2CStart();
    I2CSendSlaveAddr(BMP280_ADDR << 1);
    I2CSendData(BMP280_REG_ID);
    I2CRepeatStart();
    I2CSendSlaveAddr((BMP280_ADDR << 1)|1);
    uint8_t id = I2CRecvDataNAck();
    I2CStop();

    char buf[30];
    sprintf(buf, "Read BMP280 ID: 0x%02X\r\n", id);
    UartPuts(buf);

    if(id != 0x58) {
        UartPuts("BMP280 ID FAIL\r\n");
        return 1; // fail
    }

    UartPuts("BMP280 ID OK\r\n");

    // Read calibration registers
    bmp280_calib.dig_P1 = read16(0x88);
    bmp280_calib.dig_P2 = readS16(0x8A);
    bmp280_calib.dig_P3 = readS16(0x8C);
    bmp280_calib.dig_P4 = readS16(0x8E);
    bmp280_calib.dig_P5 = readS16(0x90);
    bmp280_calib.dig_P6 = readS16(0x92);
    bmp280_calib.dig_P7 = readS16(0x94);
    bmp280_calib.dig_P8 = readS16(0x96);
    bmp280_calib.dig_P9 = readS16(0x98);

    UartPuts("Calibration read\r\n");

    // Configure ctrl_meas register: osrs_p=1, mode=normal
    I2CStart();
    I2CSendSlaveAddr(BMP280_ADDR << 1);
    I2CSendData(BMP280_REG_CTRL_MEAS);
    I2CSendData(0x27); // osrs_p=1, mode=normal
    I2CStop();

    UartPuts("BMP280 CONFIG DONE\r\n");

    return 0; // success
}

// Read pressure in Pa (simplified, ignoring t_fine)
int32_t BMP280_ReadPressure(void) {
    I2CStart();
    I2CSendSlaveAddr(BMP280_ADDR << 1);
    I2CSendData(BMP280_REG_PRESS_MSB);
    I2CRepeatStart();
    I2CSendSlaveAddr((BMP280_ADDR << 1)|1);
    uint8_t msb = I2CRecvDataAck();
    uint8_t lsb = I2CRecvDataAck();
    uint8_t xlsb = I2CRecvDataNAck();
    I2CStop();

    int32_t adc_P = ((int32_t)msb << 12) | ((int32_t)lsb << 4) | ((xlsb >> 4) & 0x0F);

    return adc_P; // raw pressure value
}
