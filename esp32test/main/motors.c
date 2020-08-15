#include "motors.h"
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"

/*
 * DSHOT600 protocol
 * https://blck.mn/2016/11/dshot-the-new-kid-on-the-block/
 * 
 * 600 = speed in kbits / second
 * 
 * 600khz means 1.66 us per bit,
 * 
 * Each bit is transmitted as a high/low pulse
 * 0 = 555ns high, 1111ns low
 * 1 = 1111ns high, 555ns low 
 * 
 * So the clock rate we need is 1.8mhz
 * 
 * the internal clock for the rmt peripheral is 80mhz
 * so clk_div could be 80 /1.8 = 44 
 * 
 * OR set clk_div to 1, and use 44 as a value for the pulses
 * short_time = 44 ticks, ( rounding down)
 * long_time = 89 ticks (rounding up)
 *  
 */

// 80mhz / CLOCK_DIVIDER / (PULSE_SHORT_TIME + PULSE_LONG_TIME)
// Must equal 1/600 sec

#define CLOCK_DIVIDER 1
// Short time, T0H or T1L
#define PULSE_SHORT_TIME 44
// Longer pulses for T1H or T0L
#define PULSE_LONG_TIME 89


#define MOTOR0_GPIO 25
#define MOTOR1_GPIO 26

#define MOTORS_TAG "motors"

/*
 * Taken from https://github.com/gueei/DShot-Arduino/blob/master/src/DShot.cpp
 */
// top_12_bits should contain the 11 bits of throttle,
// and 1 bit of telemetry request (least significant)
// Throttle values 1..47 are reserved for commands,
// 0=stop, 1024 = half, 2047 = max
// This function shifts top_12_bits left 4 bits and fills in the crc
// in the least significant 4 bits.
static uint16_t calc_crc(uint16_t top_12_bits){
  uint8_t csum = 0;
  uint16_t csum_data = top_12_bits;
  for (uint8_t i=0; i<3; i++){
    csum ^= csum_data;
    csum_data >>= 4;
  }
  csum &= 0xf;
  return (top_12_bits<<4)|csum;
}

static void test_crc()
{
    printf("Testing calc_crc\n");
    // Test vector throttle = 1046
    // telemetry bit =0
    // crc = 0110 (0x6)
    uint16_t throttle = 1046; 
    uint16_t crctest = calc_crc(throttle << 1);
    printf("throttle = %04hx value with crc = %04hx\n",
        throttle, crctest);    
    assert(crctest == 0x826c);
}

static void motors_init_rmt()
{
    /* Initialise RMT (remote control) peripheral.
     */
    rmt_config_t config;
    memset(&config, 0, sizeof(config)); // initialise unused fields to 0
    config.rmt_mode = RMT_MODE_TX;
    config.channel = 0; // In case we use multiple channels.
    config.gpio_num = MOTOR0_GPIO;
    config.mem_block_num = 1;
    config.tx_config.idle_output_en = 1;
    config.tx_config.idle_level = 0;
    config.clk_div = CLOCK_DIVIDER;
    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
    ESP_LOGI(MOTORS_TAG, "rmt ok");

}

static void transmit_command(uint8_t motor, uint16_t top_12_bits)
{
    uint16_t command = calc_crc(top_12_bits);
    // Build 16-bit (32 pulses) of data
    rmt_item32_t pulses[16];
    uint16_t cmdbits = command;
    // Iterate through bits, most-significant first.
    for (uint8_t i=0; i<16; i++) {
        pulses[i].level0 = 1;
        pulses[i].level1 = 0;
        if (cmdbits & 0x8000) {
            // bit set
            pulses[i].duration0 = PULSE_LONG_TIME;
            pulses[i].duration1 = PULSE_SHORT_TIME;
        } else {
            // bit clear
            pulses[i].duration0 = PULSE_SHORT_TIME;
            pulses[i].duration1 = PULSE_LONG_TIME;
        }
        cmdbits = cmdbits << 1; // Shift bits left
    }
    // Write and wait.
    esp_err_t err = rmt_write_items(0, pulses, 16, true); 
    ESP_ERROR_CHECK(err);
}

void motors_init()
{
    gpio_pad_select_gpio(MOTOR0_GPIO);
    gpio_pad_select_gpio(MOTOR1_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(MOTOR0_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR1_GPIO, GPIO_MODE_OUTPUT);

    test_crc();
    
    motors_init_rmt();
    transmit_command(0,0);
}


/*
 * NB
typedef struct rmt_item32_s {
    union {
        struct {
            uint32_t duration0 :15;
            uint32_t level0 :1;
            uint32_t duration1 :15;
            uint32_t level1 :1;
        };
        uint32_t val;
    };
} rmt_item32_t;

*/
