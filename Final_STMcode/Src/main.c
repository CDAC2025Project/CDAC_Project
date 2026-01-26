 /**
 ******************************************************************************
 * FreeRTOS Monitoring Unit Firmware
 * STM32 → Sensors → OLED → UART → ESP32
 * UART format: temp,humidity,pressure\n
 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

#include "led.h"
#include "dht11.h"
#include "oled.h"
#include "font6x8.h"
#include "bmp.h"
#include "i2c.h"
#include "uart.h"

/* ===================== Data Structures ===================== */
typedef struct
{
    uint8_t temperature;
    uint8_t humidity;
} DHT11_Data_t;

typedef struct
{
    int32_t pressure_raw;
} BMP280_Data_t;

typedef struct
{
    uint8_t temperature;
    uint8_t humidity;
    int32_t pressure;
} UART_Data_t;

/* ===================== Queues ===================== */
QueueHandle_t xDHT11Queue;
QueueHandle_t xBMP280Queue;
QueueHandle_t xUARTQueue;

/* ===================== LED Task ===================== */
void vAperiodicTask(void *pvParam)
{
    for (;;)
    {
        led_toggle();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ===================== DHT11 Task ===================== */
void vDHT11Task(void *pvParam)
{
    DHT11_Data_t dhtData;
    uint8_t status;

    DHT11_Init();

    for (;;)
    {
        vTaskSuspendAll();
        status = DHT11_ReadData(&dhtData.temperature,
                                &dhtData.humidity);
        xTaskResumeAll();

        if (status)
            xQueueOverwrite(xDHT11Queue, &dhtData);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/* ===================== BMP280 Task ===================== */
void vBMP280Task(void *pvParam)
{
    BMP280_Data_t bmpData;

    I2CInit();

    if (BMP280_Init() != 0)
        vTaskDelete(NULL);

    for (;;)
    {
        /* Convert Pa → hPa (better for display) */
        bmpData.pressure_raw = BMP280_ReadPressure() / 100;

        xQueueOverwrite(xBMP280Queue, &bmpData);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ===================== OLED Task ===================== */
void vOLEDTask(void *pvParam)
{
    DHT11_Data_t dhtRx;
    BMP280_Data_t bmpRx;
    UART_Data_t uartData;

    char buf[32];

    OLED_Init();
    OLED_Clear();

    for (;;)
    {
        if (xQueueReceive(xDHT11Queue, &dhtRx, portMAX_DELAY) == pdPASS)
        {
            xQueuePeek(xBMP280Queue, &bmpRx, 0);

            OLED_Clear();

            OLED_SetCursor(0, 0);
            snprintf(buf, sizeof(buf), "Temp: %d C", dhtRx.temperature);
            OLED_PrintString(buf);

            OLED_SetCursor(2, 0);
            snprintf(buf, sizeof(buf), "Hum : %d %%", dhtRx.humidity);
            OLED_PrintString(buf);

            OLED_SetCursor(4, 0);
            snprintf(buf, sizeof(buf), "Press: %ld hPa", bmpRx.pressure_raw);
            OLED_PrintString(buf);

            /* Prepare UART packet */
            uartData.temperature = dhtRx.temperature;
            uartData.humidity    = dhtRx.humidity;
            uartData.pressure    = bmpRx.pressure_raw;

            xQueueOverwrite(xUARTQueue, &uartData);
        }
    }
}

/* ===================== UART Task (FIXED PART) ===================== */
void vUARTTask(void *pvParam)
{
    UART_Data_t rxData;
    char uartBuf[64];

    for (;;)
    {
        if (xQueueReceive(xUARTQueue, &rxData, portMAX_DELAY) == pdPASS)
        {
            /* CLEAN CSV FORMAT (for ESP parsing) */
            snprintf(uartBuf, sizeof(uartBuf),
                     "%d,%d,%ld\n",
                     rxData.temperature,
                     rxData.humidity,
                     rxData.pressure);

            UART_SendString(uartBuf);
        }
    }
}

/* ===================== main ===================== */
int main(void)
{
    led_init();

    UartInit(9600);

    xDHT11Queue  = xQueueCreate(1, sizeof(DHT11_Data_t));
    xBMP280Queue = xQueueCreate(1, sizeof(BMP280_Data_t));
    xUARTQueue   = xQueueCreate(1, sizeof(UART_Data_t));

    if (!xDHT11Queue || !xBMP280Queue || !xUARTQueue)
        while (1);

    xTaskCreate(vAperiodicTask, "LED", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    xTaskCreate(vDHT11Task, "DHT11", configMINIMAL_STACK_SIZE + 128, NULL, 3, NULL);
    xTaskCreate(vBMP280Task, "BMP280", configMINIMAL_STACK_SIZE + 128, NULL, 3, NULL);
    xTaskCreate(vOLEDTask, "OLED", configMINIMAL_STACK_SIZE + 256, NULL, 2, NULL);
    xTaskCreate(vUARTTask, "UART", configMINIMAL_STACK_SIZE + 256, NULL, 2, NULL);

    vTaskStartScheduler();

    while (1);
}
