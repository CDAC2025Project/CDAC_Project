 #ifndef I2C_H
#define I2C_H

#include "stm32f4xx.h"
#include <stdint.h>

// Public API
void I2CInit(void);

void I2CStart(void);
void I2CRepeatStart(void);
void I2CStop(void);

// Address function expects **8-bit address**
// (example: write = (addr << 1), read = (addr << 1)|1)
void I2CSendSlaveAddr(uint8_t addr8);

// Write 1 byte
void I2CSendData(uint8_t data);

// Read 1 byte with ACK
uint8_t I2CRecvDataAck(void);

// Read 1 byte with NACK (last byte)
uint8_t I2CRecvDataNAck(void);

// Simple write utility
void I2CWrite(uint8_t addr8, uint8_t data);

#endif
