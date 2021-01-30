/*
 * Beetleback ESC.
 * 
 * For attiny13a.
 * 
 * The attiny13 is widely available and jlc always have them in stock.
 */
#include "diag.h"
#include "timer.h"
#include "motor.h"
#include "defs.h"


#include <avr/io.h>

#include <util/delay.h>

#include <avr/boot.h>
#include <avr/pgmspace.h>

static const char greeting[] PROGMEM = "\r\nBeetleback\r\n\r\n";

static void clock_init()
{
    // Program clock prescaler. This is initially set from a fuse
    // but we can change it at runtime.
    // 0x0 = 1 (no scaling) 0x1 = 2 (divide by 2)
    CLKPR = (1 << CLKPCE); // We now have 4 cycles to write the value.
    CLKPR = 0x1;  
}

static const uint8_t blink_bit = 1 << 4;

static void led_on()
{
    PORTB |= blink_bit;
}

static void led_off()
{
    PORTB &= ~blink_bit;
}

/*
 * Timer ticks once every 13.33 microseconds.
 */
static const float timer_tick = 13.33;
/* Ignore any pulses outside this range.
 */
static const uint16_t PULSE_WIDTH_MIN = (uint16_t) (500.0 / timer_tick);
static const uint16_t PULSE_WIDTH_MAX = (uint16_t) (2500.0 / timer_tick);
// Nominal centre.
static const uint16_t PULSE_WIDTH_CENTRE = (uint16_t) (1500.0 / timer_tick);
static const uint16_t PULSE_WIDTH_DEAD_ZONE = ((uint16_t) (80.0 / timer_tick)) + 1;

// Time we turn of if no valid pulses...
static const uint16_t TURN_OFF_TIME = (uint16_t) (100000.0 / timer_tick);

/*
 * Before we start the motor - wait for a number of successive good
 * pulses - each which must occur within TURN_OFF_TIME of the previous.
 * 
 * This is to avoid running the motor when we receive electrical noise
 * or something, which just happens to look like a servo pulse.
 */
// Number of pulses to wait before we start up.
static const uint8_t GOOD_PULSES_STARTUP_COUNT = 5;

static uint8_t good_pulses_count;

static void dump_info()
{
    // We could print any info we want here.    
    diag_puts_progmem(PSTR("pulse min, centre, max:"));
    diag_printhex_16(PULSE_WIDTH_MIN);
    diag_putc(' ');
    diag_printhex_16(PULSE_WIDTH_CENTRE);
    diag_putc(' ');
    diag_printhex_16(PULSE_WIDTH_MAX);
    diag_newline();
    
}

static void handle_good_pulse(uint32_t width)
{
    if (good_pulses_count < GOOD_PULSES_STARTUP_COUNT) {
        good_pulses_count += 1;
        return; // DO not start yet!
    }
    led_on();
    // Check if the pulse is in the dead zone
    if (
        (width > (PULSE_WIDTH_CENTRE - PULSE_WIDTH_DEAD_ZONE)) &&
        (width < (PULSE_WIDTH_CENTRE + PULSE_WIDTH_DEAD_ZONE))
        )
    {
        // Dead zone
        motor_set_brake();
    } else {
        // drive motor.
        int16_t forward = width - (PULSE_WIDTH_CENTRE + PULSE_WIDTH_DEAD_ZONE);
        int16_t back = width - (PULSE_WIDTH_CENTRE - PULSE_WIDTH_DEAD_ZONE);
        /*
         * scale_factor - how many we multiply the pulse ticks,
         * to give a value in the range 0..255 for the motor.
         * 
         * Ideally it should go slightly over 255 so we can guarantee
         * to drive at maximum speed.
         */
        static const int16_t scale_factor = 8;
        if (forward >= 0) {
            motor_set_speed_signed(forward * scale_factor);
        } else {
            motor_set_speed_signed(back * scale_factor);            
        }
    }
    /*
    diag_puts_progmem(PSTR("pls:"));
    diag_printhex_8((uint8_t) width);
    diag_newline();
    */
}

static void handle_turn_off(uint32_t now)
{
    motor_off();
    // Low duty cycle blink:
    uint8_t blinky = ((now & 0x7fff) < 0x400);
    if (blinky) {
        led_on();
    }
    else {
        led_off();
    }
    // Reset the number of pulses we must receive before startup.
    good_pulses_count = 0;
}

/*
 * NB: calling diag_puts will turn ON the led and leave it on
 */

static void mainloop()
{
    static const uint8_t input_bit = 1 << 2; // input pin

    uint32_t last_high_edge=0;
    uint32_t last_valid_pulse_time=0;
    uint8_t last_input=0;
    
    while (1)
    {
        // Check input
        uint8_t input = PINB & input_bit;
        // record time.
        uint32_t now = timer_read();
        // low-> high: record time, it's the start of a pulse,
        // Or maybe just electrical noise?
        if (input && ! last_input) {
            last_high_edge = now;
        }
        // High -> low: measure pulse width
        if (!input && last_input) {
            uint32_t width = now - last_high_edge;
            // Pulse width good? Handle good pulse and turn motor on
            if ((width >= PULSE_WIDTH_MIN) && (width <= PULSE_WIDTH_MAX)) {
                handle_good_pulse(width);
                last_valid_pulse_time = now;
            } else {
                // Pulse width bad? ignore
            }
        }
        // Steady state: has too much time passed since last
        // valid pulse?
        // If too much time passed, then turn off motor and led.
        if ((now - last_valid_pulse_time) > TURN_OFF_TIME)
        {
            handle_turn_off(now);
        }
        last_input = input;
    }
}

int main()
{
    clock_init();
    
    DDRB |= blink_bit; // Enable output on blinky led
    
    diag_init();
    diag_puts_progmem(greeting); 
    timer_init();
    motor_init();
    dump_info();
    _delay_us(1000); // wait for the initial text to finish writing
    // and stop bits etc,
    led_off(); // keep led off at startup.
    
    mainloop(); // never returns
}
