
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

#define TELEM_LOG_SAMPLES 1500
// Interval in microseconds
#define TELEM_LOG_INTERVAL 10000  
typedef struct {
    uint16_t rpm[2];
    int cmd_speed;
    int64_t time; // Microseconds    
} telem_log_t;


static telem_log_t telem_log[TELEM_LOG_SAMPLES];
static int telem_log_pos; // position in the log.
static bool telem_log_active;
static int64_t telem_log_ts;

static void telem_log_start()
{
    memset(telem_log, 0, sizeof(telem_log));
    telem_log_pos = 0;
    telem_log_active = 1;
    telem_log_ts = esp_timer_get_time();
    printf("Telemetry log start\n");
}

static void telem_log_stop() {
    printf("Telemetry log stop\n");
    telem_log_active = 0;
    FILE * f = open_numeric_log_file();
    if (f != NULL) {
        for (int i=0; i< TELEM_LOG_SAMPLES; i++) {
            fprintf(f, "%lld %d %d %d\n",
                telem_log[i].time,
                telem_log[i].cmd_speed,                
                telem_log[i].rpm[0],
                telem_log[i].rpm[1]);
        }
        fclose(f);
    }
}

static void telem_log_loop()
{
    if (telem_log_active) {
        // If log is not full...
        if (telem_log_pos < TELEM_LOG_SAMPLES) {
            // Is it time for a new sample?
            int64_t now = esp_timer_get_time();
            if ((now - telem_log_ts) > TELEM_LOG_INTERVAL)
            {
                telem_log[telem_log_pos].time = now;
                telem_log[telem_log_pos].cmd_speed = comms_state.motor_speed;
                memcpy(telem_log[telem_log_pos].rpm, motor_telemetry_rpm, sizeof(motor_telemetry_rpm));
                telem_log_pos += 1;
                // Set time of next sample
                telem_log_ts += TELEM_LOG_INTERVAL;
            }
        }
        // If we have stopped, stop the telemetry.
        if ((comms_state.motor_speed == 0) && ( motor_telemetry_rpm[0] == 0))
        {
            telem_log_stop();
        }
    } else {
        // Telemetry inactive.
        // Should we start the telemetry log?
        if (comms_state.motor_speed > 0) {
            telem_log_start();
        } 
    }
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
            if (cmd != 0) {
                // Send the command
                motor_send_dshot_command(0, cmd);
                comms_state.pending_command = 0;
            } else {
                motor_set_speed_signed(0, comms_state.motor_speed);
                motor_set_speed_signed(1, - comms_state.motor_speed);
            }
            motor_poll_telemetry();
            telem_log_loop();
        }
        esp_task_wdt_reset();
        i += 1;
        gpio_set_level(WHITE_GPIO, (i == 2));
        // Enable the signal LED.
        gpio_set_level(SIGNAL_GPIO, comms_state.got_signal);
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
