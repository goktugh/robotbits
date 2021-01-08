/*
 * Test program for the avr attiny4 (or 5,9,10)
 * 
 * GPIO ports:
 * 0 = OC0A (pwm output)
 * 1 = OC0B (pwm output)
 *  motor will activate if 0 is high and 1 is low, or vice versa.
 *  0,1 both low => high impedence, no current in motor
 *  0,1 both high => brake enable, short motor.
 * 2 = Flashing LED, active high
 * 3 = The reset pin, and also used for
 *      servo pulses input.
 */
#include "defs.h"

#include <avr/io.h>

#include <util/delay.h>

#include <avr/pgmspace.h>

#define MAGIC_CCP_VALUE 0xD8

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
    uint16_t pwm_initial = 512;
    OCR0A = pwm_initial;
    OCR0B = pwm_initial;    
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

static void test_pwm()
{
    // CHeck the pwm works as expected.
    // Port 0 - low duty cycle
    OCR0A = 50;
    // Port 1 - high duty cycle
    OCR0B = 512-50;
    
}

int main()
{
    init_clock();
    init_timer();
    init_motor();
    uint8_t blink_bit = 1 << 2;
    DDRB |= blink_bit; // Enable output on PB0 
   
    test_pwm();
   
    uint16_t counter=0; 
    while(1) {
        PORTB |= blink_bit;
        _delay_ms(250); 
        PORTB &= ~blink_bit;
        _delay_ms(250); 
        counter += 1;
        OCR0B = (counter * 32);
        if (counter > 15) counter = 0;
    }
}
