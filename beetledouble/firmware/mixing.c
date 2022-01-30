
#include "mixing.h"
#include "motors.h"
#include "configvars.h"
#include "configpin.h"
#include "diag.h"

#include <stdbool.h>
#include <stdlib.h>

static bool mixing_on;
// Store the steering value.
static int16_t steering_last_value;

// The distance, in microseconds that we apply brake and don't drive.
#define DEAD_ZONE_US 60

static void set_channel_speed(uint8_t channel, int16_t speed)
{
    // speed in the range approx -5000 to 5000
    uint16_t pulsewidth_abs = abs(speed);
    // scale to units of 16 ticks or about 1.6us 
    uint16_t pulsewidth_scaled = pulsewidth_abs / 16;
    // It is now in the range 0..312 (approx)
    // Calculate brake zone on the same scale
    uint16_t brake_zone_scaled = ((DEAD_ZONE_US * 10) / 16);
    int16_t drive_amount = pulsewidth_scaled - brake_zone_scaled;
    uint8_t brake = (drive_amount < 0) && (config_current.braking_on);
    // clamp at max speed
    if (drive_amount > 255) drive_amount = 255;
    if (drive_amount < 0) drive_amount = 0; // Do not try to drive negatively even if braking off
    motors_commands[channel].brake = brake;
    if (brake) {
        motors_commands[channel].duty = 0;
        motors_commands[channel].direction = 0;
    } else {
        motors_commands[channel].duty = (uint8_t) drive_amount;
        motors_commands[channel].direction = (speed >= 0);
    }
}

static void do_mix(int16_t drive_speed)
{
    int16_t steer_speed = steering_last_value;
    // steer_speed should be in the range -5000 to 5000
    // clamp in case it is outside the range.
    if (steer_speed > 5000) steer_speed = 5000;
    if (steer_speed < -5000) steer_speed = -5000;
    // Scale by mixing amount
    // Avoid integer overflow; we 
    // (steer_speed * mixing_amount) might overflow (> 32768)
    steer_speed = ((steer_speed/8) * config_current.mixing_amount) / 2;
    int16_t left_speed, right_speed;
    left_speed = drive_speed + steer_speed;
    right_speed = drive_speed - steer_speed;
    set_channel_speed(0, left_speed);
    set_channel_speed(1, right_speed);
}

void mixing_set_speed(uint8_t channel, int16_t speed)
{
    // speed in the range approx -5000 to 5000
    if (mixing_on) {
        if (channel == 0) {
            // Steering.
            steering_last_value = speed;
        } else {
            // Drive
            do_mix(speed);
        }
        // TODO
    } else {
        // No mixing.
        set_channel_speed(channel, speed);
    }
}
 
void mixing_set_idle(uint8_t channel)
{
    // No pulses received on channel for a while
    if (mixing_on) {
        // Idle steering channel = set steering neutral. 
        if (channel == 0) {
            steering_last_value = 0;
        } else {
            // Idle drive channel = everything off.
            for (uint8_t c=0; c<2; c++) {
                motors_commands[c].duty = 0;
                motors_commands[c].brake = 0;                
            }
        }
    } else {
        // No mixing, deactivate idle channel only.
        motors_commands[channel].duty = 0;
        motors_commands[channel].brake = 0;
    }
}

void mixing_init()
{
    mixing_on = (bool) config_current.mixing_on;
    if (config_current.mixing_on == 2) {
        // Depends on config pin
        diag_println("mixing: checking config pin");
        mixing_on = configpin_value;
    }
    diag_println("mixing: mixing_on=%d", mixing_on);
}
