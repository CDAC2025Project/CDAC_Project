 /*
 * i2c.c
 *
 *  Created on: Mar 31, 2025
 *      Author: admin (fixed)
 */

#include "i2c.h"

/*
 * Note:
 * - This implementation expects an 8-bit address passed to I2CSendSlaveAddr().
 *   e.g. for write: (BMP280_ADDR << 1)  ; for read: (BMP280_ADDR << 1) | 1
 * - SDA/SCL must have pull-ups (4.7k typical) if your breakout doesn't include them.
 */

#ifndef BV
#define BV(x) (1U << (x))
#endif

void I2CInit(void) {
    // gpio config
    // enable gpio b clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    // set mode as alt fn (10) for PB6, PB7
    GPIOB->MODER &= ~((3U << (2*6)) | (3U << (2*7)));
    GPIOB->MODER |=  (2U << (2*6)) | (2U << (2*7));
    // set alt fn to AF4 (i2c) for PB6, PB7
    // AFR[0] contains pins 0..7
    GPIOB->AFR[0] &= ~((0xF << (4*6)) | (0xF << (4*7)));
    GPIOB->AFR[0] |= (4U << (4*6)) | (4U << (4*7));
    // set pull-up (if your breakout doesn't have pull-ups, you can enable internal)
    // Prefer external pull-ups. If needed, uncomment next two lines:
    // GPIOB->PUPDR &= ~((3U << (2*6)) | (3U << (2*7)));
    // GPIOB->PUPDR |=  (1U << (2*6)) | (1U << (2*7)); // Pull-up

    // Enable open-drain for PB6 & PB7
    GPIOB->OTYPER |= BV(6) | BV(7);

    // i2c config
    // enable i2c peri clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    // i2c sw reset
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    // clear CR1
    I2C1->CR1 = 0;

    // peri clock -- CR2 = 16MHz (change if your PCLK1 is different)
    I2C1->CR2 &= ~I2C_CR2_FREQ;
    I2C1->CR2 |= (16U << I2C_CR2_FREQ_Pos);

    // standard mode 100kHz: CCR = 80 (assumes PCLK1=16MHz)
    I2C1->CCR = 80;
    I2C1->CCR &= ~I2C_CCR_FS;    // standard mode (ensure FS=0)
    // TRISE for 16MHz -> 17
    I2C1->TRISE = 17;

    // enable ACK (default)
    I2C1->CR1 |= I2C_CR1_ACK;

    // enable i2c peripheral
    I2C1->CR1 |= I2C_CR1_PE;
}

void I2CStart(void) {
    // generate START
    I2C1->CR1 |= I2C_CR1_START;
    // wait for SB flag
    while(!(I2C1->SR1 & I2C_SR1_SB));
}

void I2CRepeatStart(void) {
    // repeated start is same as start
    I2CStart();
}

void I2CStop(void) {
    // generate STOP condition
    I2C1->CR1 |= I2C_CR1_STOP;
    // do not block here indefinitely; small delay is optional
    // return immediately; peripheral will generate STOP on bus
}

void I2CSendSlaveAddr(uint8_t addr8) {
    // write 8-bit address (including R/W bit) to DR
    I2C1->DR = addr8;
    // wait until ADDR flag is set (address phase done and ACK received)
    uint32_t timeout = 0;
    while(!(I2C1->SR1 & I2C_SR1_ADDR)) {
        timeout++;
        if(timeout > 500000) return; // timeout - caller should handle
    }
    // clear ADDR by reading SR1 and SR2
    (void)I2C1->SR1;
    (void)I2C1->SR2;
}

void I2CSendData(uint8_t data) {
    // wait TXE
    uint32_t timeout = 0;
    while(!(I2C1->SR1 & I2C_SR1_TXE)) {
        if(++timeout > 500000) return;
    }
    I2C1->DR = data;
    // wait until byte transfer finished (BTF) or TXE set
    timeout = 0;
    while(!(I2C1->SR1 & (I2C_SR1_TXE | I2C_SR1_BTF))) {
        if(++timeout > 500000) return;
    }
    // if BTF, all good
    while(!(I2C1->SR1 & I2C_SR1_BTF)) {
        // small loop to ensure transfer completes; optional
        break;
    }
}

uint8_t I2CRecvDataAck(void) {
    // Ensure ACK is enabled
    I2C1->CR1 |= I2C_CR1_ACK;
    // clear POS bit (we're not using POS here)
    I2C1->CR1 &= ~I2C_CR1_POS;

    // wait until data received
    uint32_t timeout = 0;
    while(!(I2C1->SR1 & I2C_SR1_RXNE)) {
        if(++timeout > 500000) return 0;
    }
    // read and return
    return (uint8_t)I2C1->DR;
}

uint8_t I2CRecvDataNAck(void) {
    // Disable ACK so NACK is sent after next byte (last byte)
    I2C1->CR1 &= ~I2C_CR1_ACK;
    // generate STOP immediately (for master receiver, this ensures bus releases)
    I2C1->CR1 |= I2C_CR1_STOP;

    // wait until data received
    uint32_t timeout = 0;
    while(!(I2C1->SR1 & I2C_SR1_RXNE)) {
        if(++timeout > 500000) return 0;
    }
    // read data
    uint8_t val = (uint8_t)I2C1->DR;

    // re-enable ACK for subsequent reads
    I2C1->CR1 |= I2C_CR1_ACK;

    return val;
}

void I2CWrite(uint8_t addr8, uint8_t data) {
    I2CStart();
    I2CSendSlaveAddr(addr8); // addr8 includes R/W
    I2CSendData(data);
    I2CStop();
}
