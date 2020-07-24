
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "driver/gpio.h"

#include "comms.h"


#define BLINK_GPIO 2
#define OTHER_GPIO 13


static void init_misc()
{
    printf("init_misc()");
    //Initialize NVS (flash)
    printf("Initialising NVS\n");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
}

void app_main()
{
    init_misc();
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Initialising comms\n");
    comms_init();    

    printf("Setting gpio up for led\n");
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_pad_select_gpio(OTHER_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(OTHER_GPIO, GPIO_MODE_OUTPUT);

    int i = 0;
    while (1) {
        gpio_set_level(BLINK_GPIO, (i % 2) );
        gpio_set_level(OTHER_GPIO, ! (i % 2) );
        printf("Loop %d...\n", i);
        fflush(stdout);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        i += 1;
    }
    // Resets the board: esp_restart();
}
