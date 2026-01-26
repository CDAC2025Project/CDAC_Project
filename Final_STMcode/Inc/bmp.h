 #ifndef BMP_H_
#define BMP_H_

#include "stm32f4xx.h"


// I2C address of BMP280
#define BMP280_ADDR      0x76  // Use 0x77 if SDO=VCC

// BMP280 registers
#define BMP280_REG_ID        0xD0
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_PRESS_MSB 0xF7

// Calibration structure
typedef struct {
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} BMP280_Calib;

// Globals
extern BMP280_Calib bmp280_calib;
extern int32_t t_fine;

// Function prototypes
uint8_t BMP280_Init(void);
int32_t BMP280_ReadPressure(void);

#endif /* BMP_H_ */
