 #ifndef DHT11_H_
#define DHT11_H_

#include "stm32f4xx.h"
#include <stdint.h>

#define DHT11_PORT GPIOA
#define DHT11_PIN  0

void DHT11_Init(void);
uint8_t DHT11_Read(uint8_t *temperature, uint8_t *humidity);

#endif
