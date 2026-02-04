#ifndef BMP_H_
#define BMP_H_

#include "stm32f4xx.h"
#include "i2c.h"
#include "uart.h"

// I2C address of BMP280
#define BMP280_ADDR      0x76   
// BMP280 registers
#define BMP280_REG_ID        0xD0
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_PRESS_MSB 0xF7

/ 
// Function prototypes
uint8_t BMP280_Init(void);
int32_t BMP280_ReadPressure(void);

#endif /* BMP_H_ */
