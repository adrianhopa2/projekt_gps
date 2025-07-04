#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "parser_nmea.h"
#include "display_data.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#include "esp_sleep.h"

#define I2C_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_TICKS_TO_WAIT 100

static const char *TAG = "UART TEST";

QueueHandle_t uart_queue;

parser_t parser;
SSD1306_t dev;

SemaphoreHandle_t xSem;

uint8_t buffer[BUFF_SIZE];

#define GPS_UART_PORT_NUM      UART_NUM_2
#define GPS_UART_TX_PIN        17     // TX 
#define GPS_UART_RX_PIN        16     // RX

uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

static void uart_init(){
    uart_driver_install(GPS_UART_PORT_NUM, 1024 * 2, 0, 16, &uart_queue, 0);
    uart_param_config(GPS_UART_PORT_NUM, &uart_config);
    uart_set_pin(GPS_UART_PORT_NUM, GPS_UART_TX_PIN, GPS_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_enable_pattern_det_baud_intr(GPS_UART_PORT_NUM, '\r', 1, 9, 0, 0);
    uart_pattern_queue_reset(GPS_UART_PORT_NUM, 16);
    uart_flush(GPS_UART_PORT_NUM);
}

void display_task(void* parameter)
{
    xSemaphoreTake(xSem, 0);

    while(1){
        if(pdTRUE == xSemaphoreTake(xSem, portMAX_DELAY))
        {
            //ESP_LOGI(TAG, "\tDisplayed");
            display_data(&dev, &parser);
        }
    }
}

void parser_task(void* parameter)
{   

    while (1){
        int pos = uart_pattern_pop_pos(GPS_UART_PORT_NUM);
        //ESP_LOGI(TAG, "pos: %d", pos);
        if (pos != -1) {
            int read_len = uart_read_bytes(GPS_UART_PORT_NUM, buffer, pos + 1, 100 / portTICK_PERIOD_MS);
            if (read_len > 0 && read_len < sizeof(buffer)) {
                buffer[read_len] = '\0';
                separate_items(&parser, (char*) buffer);
                gps_decode(&parser);
            }
            xSemaphoreGive(xSem);
        } 
        else {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

    }
}

void app_main(void)
{

    gpio_num_t CONFIG_GPIO_SDA = GPIO_NUM_21;
    gpio_num_t CONFIG_GPIO_SCL = GPIO_NUM_22;
    gpio_num_t CONFIG_GPIO_RES = GPIO_NUM_15;

	i2c_master_init(&dev, CONFIG_GPIO_SDA, CONFIG_GPIO_SCL, CONFIG_GPIO_RES);
    ssd1306_init(&dev, 128, 64);

	init_screen(&dev);
	uart_init();

	xSem = xSemaphoreCreateBinary();

    xTaskCreate(parser_task, "Parser task", 4096, NULL, 2, NULL);
    xTaskCreate(display_task, "Display task", 4096, NULL, 1, NULL);

    vTaskDelete(NULL);

    vTaskStartScheduler();
}
