
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "nvs_flash.h"

#include "driver/gpio.h"

#include "utils.h"
#include "comms.h"
#include "fs.h"
#include "motors.h"

#define BLINK_GPIO 2
#define OTHER_GPIO 13


static void init_misc()
{
    printf("init_misc()\n");
    //Initialize NVS (flash)
    printf("Initialising NVS\n");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
}

static void main_loop()
{
    // Add current task to watchdog.
    esp_task_wdt_add(NULL);
    printf("Starting main_loop()\n");
    int i = 0;
    while (1) {
        printf("Loop %d...\n", i);
        fflush(stdout);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        for (int j=0; j<250; j++) {
            busy_sleep(2 * 1000);
            int cmd = comms_state.pending_command;
            bool send_telemetry = ((j% 50) == 0);
            if (cmd != 0) {
                // Send the command
                motor_send_dshot_command(0, cmd);
                comms_state.pending_command = 0;
            } else {
                motor_set_speed_signed(0, comms_state.motor_speed, send_telemetry);
            }
        }
        esp_task_wdt_reset();
        i += 1;
    }    
}

static void main_init()
{
    printf("initialising misc things...\n");
    init_misc();
    
    printf("initialising filesystem...\n");
    fs_init();

    printf("Initialising comms...\n");
    comms_init();    
    web_server_init();

    printf("Setting gpio up for led...\n");
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_pad_select_gpio(OTHER_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(OTHER_GPIO, GPIO_MODE_OUTPUT);
    printf("Initialising motors...\n");
    motors_init();
}


static void main_task(void *pvParameters)
{
    main_init();
    main_loop();
}

void app_main()
{
    init_misc();
    printf("Hello Melty world!\n");
    printf("Starting main task\n");

    uint32_t core = 1;
    xTaskCreatePinnedToCore(main_task, "main_task", 4096, NULL, 5, NULL, core);
}
