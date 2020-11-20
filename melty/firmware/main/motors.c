#include "motors.h"
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "driver/uart.h"
#include "utils.h"

#include <stdbool.h>
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
 * DSHOT150
 * 150 = speed in kbits / sec
 * 6.6us per bit
 * high time = 4.4us, 355 80mhz ticks
 * low time = 2.2us, 178 80mhz ticks
 *  
 */

// 80mhz / CLOCK_DIVIDER / (PULSE_SHORT_TIME + PULSE_LONG_TIME)
// Must equal 1/600 sec

#define CLOCK_DIVIDER 1
// dshot600
// Short time, T0H or T1L
#define PULSE_SHORT_TIME 44
// Longer pulses for T1H or T0L
#define PULSE_LONG_TIME 89
//dshot 150
// #define PULSE_SHORT_TIME 178
// #define PULSE_LONG_TIME 355

#define NUM_MOTORS 2

// Minimum time between dshot commands:
// A dshot command takes 16 * 1/600000 sec =~ 26 us
#define MIN_DSHOT_INTERVAL_US 50
// Time to wait before we send another dshot command
// even if nothing has changed, because we should keep the esc
// awake so it doesn't disarm.
// (I don't know where this is documented but it is typically 20ms
// for PWM)
#define DSHOT_REPEAT_INTERVAL_US 5000
// Time between telemetry, needs to be long enough that telemetry
// packet is complete. Telemetry packets are 10 bytes long
// 10 bytes @115k baud, 10 bits per packet =~ 1 ms transmit time.
// leave a gap between.
// Note that telemetry is shared between all motors, so we need to
// only request telemetry on one motor at once, and remember which
// motor it was.
#define TELEMETRY_INTERVAL 25000 

#define MOTOR0_GPIO 25
#define MOTOR1_GPIO 26
#define MOTOR0_PWM_GPIO 27
#define MOTOR1_PWM_GPIO 33
#define TELEMETRY_GPIO 35
// for diag messages
#define MOTORS_TAG "motors"

typedef struct {
    int last_speed_signed;
    int64_t last_send_time; // from esp_timer_get_time in microseconds
} motor_state_t;

static motor_state_t motor_state[NUM_MOTORS];
// Uart instance which we use for telemetry. uart0 is used for
// the normal diagnostic printf etc.
static const int telemetry_uart=1; 
static bool telemetry_expected; // true= expecting a telemetry packet.
// Last motor we requested telemetry.
static int telemetry_last_motor;
int64_t telemetry_last_send_time; 
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
    assert(crctest == 0x82c6);
}

static void motors_init_rmt()
{
    /* Initialise RMT (remote control) peripheral.
     */
    for (uint8_t motor = 0; motor < 2; motor ++) {
        rmt_config_t config;
        memset(&config, 0, sizeof(config)); // initialise unused fields to 0
        config.rmt_mode = RMT_MODE_TX;
        config.channel = motor; // channel0 for motor0, c1 for motor1
        config.gpio_num = (motor==1) ? MOTOR1_GPIO : MOTOR0_GPIO;
        config.mem_block_num = 1;
        config.tx_config.idle_output_en = 1;
        config.tx_config.idle_level = 0;
        config.clk_div = CLOCK_DIVIDER;
        ESP_LOGI(MOTORS_TAG, "Initialising rmt %d", motor);
        ESP_ERROR_CHECK(rmt_config(&config));
        ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
        ESP_LOGI(MOTORS_TAG, "rmt %d ok", config.channel);
        // Configure RMT channels 2,3 to generate servo pulses instead.
        memset(&config, 0, sizeof(config)); // initialise unused fields to 0
        config.rmt_mode = RMT_MODE_TX;
        config.channel = motor+2; // channel2 for motor0, c3 for motor1
        config.gpio_num = (motor==1) ? MOTOR1_PWM_GPIO : MOTOR0_PWM_GPIO;
        config.mem_block_num = 1;
        config.tx_config.idle_output_en = 1;
        config.tx_config.idle_level = 0;
        config.clk_div = 80; // Set for microseconds.
        ESP_ERROR_CHECK(rmt_config(&config));
        ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
        ESP_LOGI(MOTORS_TAG, "rmt %d ok", config.channel);
    }
}

static void motors_init_telemetry()
{
    /*
     * Initialise uart peripheral on telemetry port.
     */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    ESP_ERROR_CHECK(uart_param_config(telemetry_uart, &uart_config));
    // uart will only need rx, not anything else.
    ESP_ERROR_CHECK(uart_set_pin(telemetry_uart, 
        UART_PIN_NO_CHANGE, // TX pin
        TELEMETRY_GPIO, // RX pin
        UART_PIN_NO_CHANGE, // rts
        UART_PIN_NO_CHANGE  // cts
        ));
    const int buf_size = UART_FIFO_LEN*2;
    ESP_ERROR_CHECK(uart_driver_install(telemetry_uart, buf_size, 0, 0, NULL, 0));
    ESP_LOGI(MOTORS_TAG, "motors_init_telemetry done");

}

static void send_pulses(uint8_t motor, uint16_t pulses_int)
{
    uint16_t cmdbits = pulses_int;
    // Build 16-bit (32 pulses) of data
    rmt_item32_t pulses[16];
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
    esp_err_t err = rmt_write_items(motor, pulses, 16, true); 
    ESP_ERROR_CHECK(err);    
}

static void transmit_command(uint8_t motor, uint16_t top_12_bits)
{
    uint16_t command = calc_crc(top_12_bits);
    send_pulses(motor, command);
}

static void transmit_command_all(uint16_t top_12_bits)
{
    transmit_command(0,top_12_bits);
    transmit_command(1,top_12_bits);    
}

static void send_200ms_stuff(uint16_t cmd)
{
    for (int n=0; n<100; n++) {
        transmit_command_all(cmd);
        busy_sleep(2000); // 2 ms
    }    
}

static void motor_send_dshot_command_all(uint16_t cmd)
{
    uint16_t top_12_bits = (cmd << 1 );
    // Set tele. flag
    top_12_bits |= 1;
    for (int n=0; n<20; n++) {
        transmit_command(0, top_12_bits);
        transmit_command(1, top_12_bits);
        busy_sleep(2000); // 2 ms
    }        
}

// See:
// https://github.com/bitdump/BLHeli/blob/master/BLHeli_32%20ARM/BLHeli_32%20Firmware%20specs/Digital_Cmd_Spec.txt
#define DSHOT_CMD_3D_MODE_ON 10
#define DSHOT_CMD_LED0_ON 22
#define DSHOT_CMD_LED1_ON 23
#define DSHOT_CMD_LED2_ON 24
#define DSHOT_CMD_LED0_OFF 26
#define DSHOT_CMD_LED1_OFF 27
#define DSHOT_CMD_LED2_OFF 28

void motors_init()
{
    // Note, it is not necessary to enable outputs,
    // the rmt peripheral or library does it anyway.

    test_crc();
    
    motors_init_rmt();
    motors_init_telemetry();
    // Try to get the ESC going...
    // Initialise dshot protocol by sending some 0 words
    // Start with nothing every 2 ms.
    // We need to wait for the startup beep.
    printf("motors_init: Waiting for motor startup beep\n");
    for (int n=0; n< 15; n++) {
        send_200ms_stuff(0);
    }
    printf("motors_init: Sending initialisation commands\n");
    // Start again
    // send_200ms_stuff(0);
    // Now try a (high throttle)
    // send_200ms_stuff(600);
    // Then nothing again.
    send_200ms_stuff(0);
    // Now set 3d mode (many times)
    motor_send_dshot_command_all(DSHOT_CMD_3D_MODE_ON);
    send_200ms_stuff(0);
    motor_send_dshot_command_all(DSHOT_CMD_3D_MODE_ON);
    // Set the funky light colours
    motor_send_dshot_command_all(DSHOT_CMD_LED0_OFF);
    motor_send_dshot_command_all(DSHOT_CMD_LED1_OFF);
    motor_send_dshot_command_all(DSHOT_CMD_LED2_OFF);
    // Now set different colours for M0 and M1
    motor_send_dshot_command(0, DSHOT_CMD_LED0_ON);
    motor_send_dshot_command(1, DSHOT_CMD_LED1_ON);
    printf("motors_init: Finished \n");
    // After motors_init is finished we must continue to send commands
    // to the esc, at least every 10ms (or something?) otherwise it will
    // disarm, power down and reset it settings (e.g. 3d mode, led colour)
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

static void motor_set_speed_dshot(uint8_t motor, int speed_signed, bool send_telemetry)
{
    // dshot commands:
    // 0= off
    // 1..47 reserved (config etc)
    // 48..1047 = forward speeds
    // 1049..2047 = reverse speeds
    // 1048=?
    uint16_t dshot = 0; // motor off
    if (speed_signed > 0) {
        dshot = 47 + speed_signed;
        if (dshot > 1047) dshot = 1047;
    }
    if (speed_signed < 0) {
        dshot = 1048 - speed_signed;
        if (dshot > 2047) dshot = 2047;
    }
    uint16_t top_12_bits = (dshot << 1 );
    if (send_telemetry) { // add telemetry bit
        top_12_bits |= 1;
        // Clear any old rubbish in the buffer for telemetry.
        uart_flush_input(telemetry_uart);
        telemetry_expected = true;
    }
    transmit_command(motor, top_12_bits);
    // Send PWM pulses
    rmt_item32_t pulses[16];
    pulses[0].level0 = 1;
    pulses[0].level1 = 0;
    pulses[0].duration0 = speed_signed + 1500; // Microseconds
    pulses[0].duration1 = 2000;
    rmt_write_items(motor+2, pulses, 1, false); // Do not wait.
}

void motor_set_speed_signed(uint8_t motor, int speed_signed)
{
    int64_t now = esp_timer_get_time();
    int64_t interval = (now - motor_state[motor].last_send_time);
    if (interval < MIN_DSHOT_INTERVAL_US) {
        // Do not send too often! otherwise the previous
        // command might not be finished, or the esc might not have
        // time to process.
        return;
    }
    if ((interval > DSHOT_REPEAT_INTERVAL_US) || (speed_signed != motor_state[motor].last_speed_signed)) {
        // If we want to repeat the command, OR
        // the throttle is different:
        bool send_telemetry = false;
        // Only send telemetry if there is not telemetry in progress
        // and we have not requested telemetry already
        int64_t telemetry_age = now - telemetry_last_send_time;
        if (telemetry_age > TELEMETRY_INTERVAL) {
            send_telemetry = true;
            telemetry_last_send_time = now;
        }
        motor_set_speed_dshot(motor, speed_signed, send_telemetry);
        motor_state[motor].last_speed_signed = speed_signed;
        motor_state[motor].last_send_time = now;
    }
}
    
void motor_send_dshot_command(uint8_t motor, int cmd)
{
    uint16_t top_12_bits = (cmd << 1 );
    // Set tele. flag
    top_12_bits |= 1;
    transmit_command(motor, top_12_bits);
}

/*
 * Telemetry see
 * http://ultraesc.de/downloads/KISS_telemetry_protocol.pdf
 * 
 * 10 bytes long binary, with a checksum or something.
 * 
 * Telemetry uses big-endian format.
 * byte offset 7,8 = ERPM
 */
#define TELEMETRY_PACKET_LEN 10

static void handle_telemetry_packet(uint8_t *telemetry_buf);

void motor_poll_telemetry()
{
    // check if we have received telemetry in the buffer
    size_t len;
    if (uart_get_buffered_data_len(telemetry_uart, &len) == ESP_OK) {
        // Ignore any data if no telemetry is expected yet.
        if ((len >= TELEMETRY_PACKET_LEN) && telemetry_expected) {
            // Get telemetry data immediately with no waiting.
            uint8_t telemetry_buf[TELEMETRY_PACKET_LEN];
            int res = uart_read_bytes(telemetry_uart, telemetry_buf, TELEMETRY_PACKET_LEN, 0);
            // If telemetry is valid
            if (res == TELEMETRY_PACKET_LEN) {
                // Handle it somehow
                handle_telemetry_packet(telemetry_buf);
            }
        }
    }
}

uint8_t update_crc8(uint8_t crc, uint8_t crc_seed)
{
    uint8_t crc_u, i;
    crc_u = crc;
    crc_u ^= crc_seed;
    for ( i=0; i<8; i++) 
        crc_u = ( crc_u & 0x80 ) ? 0x7 ^ ( crc_u << 1 ) : ( crc_u << 1 );
    return (crc_u);
}

uint8_t get_crc8(uint8_t *Buf, uint8_t BufLen)
{
    uint8_t crc = 0, i;
    for( i=0; i<BufLen; i++) 
        crc = update_crc8(Buf[i], crc);
    return (crc);
}

static void handle_telemetry_packet(uint8_t *telemetry_buf)
{
    uint8_t expected_crc = get_crc8(telemetry_buf, 9);
    uint8_t got_crc = telemetry_buf[9];
    if (got_crc != expected_crc) {
        printf("crc fail expected: %02x got: %02x", expected_crc, got_crc);
        return;
    }
    // Erpm / 100
    uint16_t erpm = ((telemetry_buf[7]) << 8) + telemetry_buf[8];
    // Batt. voltage
    uint16_t volts = ((telemetry_buf[1]) << 8) + telemetry_buf[2];
    printf("Telem: erpm=%04d x100 volts=%04d /100 \n", erpm, volts);
    telemetry_expected = false;
}
