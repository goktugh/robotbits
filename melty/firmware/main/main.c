
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
#include "controller.h"

#define SIGNAL_GPIO 21
#define WHITE_GPIO 23


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
        int motor_speed=0;
        for (int j=0; j<250; j++) {
            busy_sleep(2 * 1000);
            motor_speed = (controller_state.spin_throttle * 10);
            motor_set_speed_signed(0, motor_speed);
            motor_set_speed_signed(1, - motor_speed);
            motor_poll_telemetry();
        }
        esp_task_wdt_reset();
        i += 1;
        gpio_set_level(WHITE_GPIO, (i == 2) || controller_state.led_test);
        // Enable the signal LED.
        gpio_set_level(SIGNAL_GPIO, controller_state.got_signal);
        
        printf("Motor speed %d\n", motor_speed);
    }    
}

static void main_init()
{
    printf("Setting gpio up for led...\n");
    gpio_pad_select_gpio(SIGNAL_GPIO);
    gpio_pad_select_gpio(WHITE_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(SIGNAL_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(WHITE_GPIO, GPIO_MODE_OUTPUT);

    printf("initialising filesystem...\n");
    fs_init();

    printf("Initialising comms...\n");
    comms_init();    
    web_server_init();
    
    printf("Initialising controller\n");
    controller_init();

    printf("Initialising motors...\n");
    motors_init();
}


static void main_task(void *pvParameters)
{
    printf("main_task startup\n");
    main_init();
    main_loop();
}

void app_main()
{
    printf("Hello Melty world!\n");
    printf("In app_main\n");
    printf("initialising misc things...\n");
    init_misc();
    
    uint32_t core = 1;
    xTaskCreatePinnedToCore(main_task, "main_task", 4096, NULL, 5, NULL, core);
}
