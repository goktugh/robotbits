/*
 * N20 Motor driver firmware for avr attiny4 (or 5,9,10)
 * 
 * GPIO ports:
 * 0 = OC0A (pwm output)
 * 1 = OC0B (pwm output)
 *  motor will activate if 0 is high and 1 is low, or vice versa.
 *  0,1 both low => high impedence, no current in motor
 *  0,1 both high => brake enable, short motor.
 * 2 = LED, active high
 * 3 = The reset pin, and also used for
 *      servo pulses input.
 */
#include "defs.h"

#include <avr/io.h>

#include <util/delay.h>

#include <avr/pgmspace.h>

#include <stdlib.h>

// Input GPIO bit
#define INPUT_BIT (1<<3)

#define MAGIC_CCP_VALUE 0xD8

/*
 * Pulses outside these ranges are ignored.
 */
#define MIN_PULSE_US 750
#define MAX_PULSE_US 2250

// Number of timer overflows to wait before stopping motor,
// if we receive no pulses
// (approx 500 per second)
#define TURN_OFF_TIME_OVERFLOWS (100)

static void init_clock()
{
    // CLKPSR controls the clock prescaler
    // default is 8x which is value 0x3
    // 0x0 means 1x
    // 0x1 means 2x
    // 0x2 means 4x
    // Unlock protected io
    CCP = MAGIC_CCP_VALUE;
    CLKPSR = 0x1;
    // Should now be 4mhz
}

static void motor_off()
{
    // This should make both motor drive pins remain low.
    uint16_t pwm_initial = 0;
    OCR0A = pwm_initial;
    OCR0B = pwm_initial;    
}

#define BLINK_BIT (1 << 2) // LED 

static void led_on()
{
    PORTB |= BLINK_BIT;
}

static void led_off()
{
    // Clear the bit
    PORTB &= ~ BLINK_BIT;
}

static void init_timer()
{
    // Timer - will count up from 0 to 0x3ff in "fast PWM" mode.
    // 10-bit "fast pwm" mode.
    // one tick every 2 microseconds, so if the system clock
    // is 4mhz, with divider=8
    //
    // Total period will be 1024 ticks, so 2048 us
    // Or about 488hz pwm
    
    // set TCRR0A
    uint8_t waveform_mode = 0x7; // fast pwm, 10bit
    uint8_t clock_scale = 0x2; // 0x2 = factor of 8, 0x8 = factor of 256
    // Compare mode - 0b10 for compare A and B
    uint8_t compare_mode = 0x2;
    // TCRR0A contains the two compare modes, and the
    // bottom 2 bits of waveform_mode
    TCCR0A = (compare_mode << 6) | (compare_mode << 4) |
        (waveform_mode & 0x3);
    // tcrr0b contains 3 bits of clock scale and two more bits of waveform_mode
    // (and some other stuff we don't care about and set to 0)
    TCCR0B = clock_scale | ((waveform_mode >> 2) << 3);
    // TCCR0C contains "force output compare" which we ignore.
    // Now to set PWM duty cycle, we can just write to 
    // OCR0A and OCR0B 
    // Prescaler reset. Do we need to trigger this?
    GTCCR = 0x1; // Assert the prescaler reset; self-clears.
    
    // To set a low duty cycle, we set a low value (0=off)
    // To set a high duty cycle, we set a high value (512=max)
}

static void init_motor()
{
    // Set the two ports to output
    motor_off();
    DDRB |= 0x3; // 0 and 1
}

// Number of times the timer has overflowed since last
// positive edge.
static uint8_t timer_overflow_count_since_positive_edge;
// Number of timer overflows since last valid pulse.
static uint16_t timer_overflow_count_since_pulse;
static uint16_t ticks_at_positive_edge;
static uint8_t flash_count;
/*
 * Previous input state, 0 or 1
 * 1=high
 */
static uint8_t previous_input_state;

/*
 * Timer (16-bit) overflow, called about every
 * 2048 microseconds.
 */
static void handle_timer_overflow()
{
    timer_overflow_count_since_positive_edge += 1;
    timer_overflow_count_since_pulse += 1;
    if (timer_overflow_count_since_positive_edge > 20) {
        // Too many
        timer_overflow_count_since_positive_edge = 20;
    }
    if (timer_overflow_count_since_pulse >= TURN_OFF_TIME_OVERFLOWS) {
        // Too long since we got a valid pulse.
        timer_overflow_count_since_pulse = TURN_OFF_TIME_OVERFLOWS;
        motor_off();
        // Blink led with low duty so the user knows we're still powered on but
        // no signal.
        flash_count = (flash_count + 1) & 0x3f; 
        if (flash_count < 4)
            led_on();
        else
            led_off();
    }
}

#define PULSE_CENTRE 1500
#define DEADZONE 100

static int16_t scale_int16(int16_t n)
{
    /* scale up by a fixed factor without using multiply. */
    int16_t fivetimes = (n << 2) + n;
    return fivetimes >> 2;
}

static void handle_pulse(uint16_t width)
{
    // Motor settings, 512=max
    uint16_t m0=0, m1=0;
    // determine if we're in dead zone...
    if ((width > (PULSE_CENTRE - DEADZONE)) && (width < (PULSE_CENTRE + DEADZONE)))
    {
        // Centre braking.
        m0 = m1 = 512;
    } else {
        int16_t pulse_signed = ((int16_t) width) - PULSE_CENTRE;
        // Scale pulse so that it goes to > 500 
        int16_t pulse_scaled = scale_int16(abs(pulse_signed));
        // Cap this at the ceiling
        if (pulse_scaled > 512) pulse_scaled = 512;
        if (pulse_signed > 0) {
            m0 = pulse_scaled;
        } else {
            m1 = pulse_scaled;
        }
    }
    // Set motor speed forward, reverse or brake
    OCR0A = m0;
    OCR0B = m1;
    timer_overflow_count_since_pulse = 0;
    led_on();
}

static void check_input(uint16_t ticks_now)
{
    uint8_t input = DDRB & INPUT_BIT;
    if (input && (! previous_input_state)) {
        // Positive edge.
        timer_overflow_count_since_positive_edge = 0;
        ticks_at_positive_edge = ticks_now;
    }
    if (previous_input_state && (! input)) {
        // Negative edge
        // Measure pulse width:
        // (could be negative)
        int32_t width = ((int32_t) ticks_now - ticks_at_positive_edge);
        // Add number of overflows
        width += (((int32_t) timer_overflow_count_since_positive_edge) << 16);
        // width is in timer ticks
        // Timer ticks are 2us
        width *= 2; // width is now in microseconds
        // So we expect a reasonable value between 1000us and 2000us
        if ((width < MIN_PULSE_US) || (width > MAX_PULSE_US)) {
            // Bad pulse width, ignore.
        } else {
            handle_pulse(width);
        }
    }
    previous_input_state = input;
}

static void mainloop()
{
    uint16_t previous_ticks = 0;
    while (1) {
        uint16_t ticks;
        ticks = TCNT0;
        if (ticks < previous_ticks) {
            handle_timer_overflow();
        }
        check_input(ticks);
        
        previous_ticks = ticks;
    }
}

int main()
{
    init_clock();
    init_timer();
    init_motor();
    DDRB |= BLINK_BIT; // Enable output on PB0 

    mainloop();
}
