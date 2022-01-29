#include <avr/io.h>

#include "diag.h"
#include "motors.h"
#include "mixing.h"
#include <stdlib.h>

#define CHANNEL_COUNT 2

/*
 * ROUTE:
 * 
 * PWMIN1 -> PB3 -> ASYNCCH1 -> TCB0
 * PWMIN2 -> PA3 -> ASYNCCH0 -> TCB1
 * 
 * So we use timer 0 for ch1 timer 1 for ch2, as you'd expect.
 * 
 * The only things which are backwards are asynch channels.
 */

void rxin_init()
{
    // Set the ports as inputs
    
    PORTA.DIRCLR = 1 << 3; // PA3 
    PORTB.DIRCLR = 1 << 3;  // PB3
    
    // (see datasheet 14.5.3 Sync  Channel n Generator Selection)
    // Configure ASYNC event 1 to read PWMIN1 
    // Which will be routed to TCB0
    // From PIN PB3
    EVSYS.ASYNCCH1 = 0x0d; 
    EVSYS.ASYNCUSER0 = 0x4; // Route TCB0 from ASYNCCH1

    // Configure ASYNC event 0 to read PWMIN2 from PA3 
    // Which will be routed to TCB1
    EVSYS.ASYNCCH0 = 0x0d; 
    EVSYS.ASYNCUSER11 = 0x3; // Route to TCB1 from ASYNCCH0
    
    // Configure TCB0 and TCB1 identically, except they will
    // take their input from different pins.
    // We can poll TCBn.INTFLAGS to detect when an input capture
    // is ready, then we read TCBn.CCMP which will auto-clear the
    // INTFLAGS 
    void init_tcb(TCB_t *tcb) {
        // Clock: divide the system clock by 2,
        // So it's counting in 20mhz / 2 100ns ticks
        // So it wraps after (65536 / 10) microseconds 
        tcb->CTRLA = (0x1 << 1); // CLKSEL CLK_PER/2
        // Set count-mode
        tcb->CTRLB = 0x4; // Pulse-width measurement
        tcb->EVCTRL = 0x1; // CAPTEI event enable.
        tcb->CNT = 0;
        tcb->INTFLAGS = 0x1; // clear any existing int
        tcb->CTRLA |= 0x1; // set ENABLE
    }
    
    init_tcb(&TCB0);
    init_tcb(&TCB1);
}

static bool timer_has_overflowed[CHANNEL_COUNT];

//Number of ticks, in 100ns ticks, to consider the timer
//overflowed (ignore)
#define COUNT_OVERFLOW 25000
// Number of ticks - minimum length of valid pulse.
#define COUNT_MIN 4000

// Number of timer loops since the last pulse on each channel.
static uint8_t ticks_since_last_pulse[CHANNEL_COUNT];

#define GOOD_PULSE_MIN 10
static uint8_t good_pulse_count[CHANNEL_COUNT];

void rxin_loop()
{
    void check_timer(TCB_t *tcb, uint8_t index) {
        // Check if the event has fired?
        if (tcb->INTFLAGS & 0x1) {
            // Capture event has happened.
            // Reading this register clears INTFLAGS
            uint16_t pulsewidth = tcb->CCMP;
            if (timer_has_overflowed[index] || (pulsewidth < COUNT_MIN) ) {
                // Invalid pulse.
                timer_has_overflowed[index] = 0;
                // Ignore the pulse;
                // Reset the good pulse count, so we are less affected by
                // electrical noise.
                good_pulse_count[index] = 0;
                return;
            }
            // Do not turn the motors on until we receive GOOD_PULSE_MIN pulses.
            if (good_pulse_count[index] < GOOD_PULSE_MIN) {
                good_pulse_count[index] += 1;
                return; // Do not activate.
            }
            
            // This is in units of 100ns,
            // we need to divide by 10 to get microseconds,
            /* This debug is handy but messes up motor drive */
#if 0
            uint16_t pulsewidth_us = pulsewidth / 10;
            diag_println("p %c %04x", index + '0', pulsewidth_us);
#endif
            // Let's find a more reasonable range,
            int16_t pulsewidth_signed = pulsewidth;
            pulsewidth_signed -= 15000; 
            // Now in the range approx -5000 to 5000
            mixing_set_speed(index, pulsewidth_signed);
            ticks_since_last_pulse[index] = 0;
        } else {
            // Get signal
            uint8_t pin;
            if (index == 0) {
                pin = PORTB.IN & (1 << 3); // PB3
            } else {
                pin = PORTA.IN & (1 << 3); // PA3                
            }
            
            // Check for timer overflow?
            if (pin && (tcb->CNT > COUNT_OVERFLOW))
            {
                timer_has_overflowed[index] =1 ;
            }
        }
    }
    
    check_timer(&TCB0, 0);
    check_timer(&TCB1, 1);
}

/*
 * Stop motors if channel is idle (no pulses)
 * TCA wraps in about 26 ms,
 * 
 * Most receivers send pulses approximately every 20ms,
 * We give them a lot of grace time before we shut off.
 */
#define IDLE_TICKS 8

// Called when TCA overflows.
void rxin_timer_overflow()
{
    for (uint8_t i=0; i<CHANNEL_COUNT; i++) {
        ticks_since_last_pulse[i] += 1;
        if (ticks_since_last_pulse[i] >= IDLE_TICKS) {
            // Idle timeout this channel.
            mixing_set_idle(i);
            ticks_since_last_pulse[i] = 0;
            // Reset the good pulse counter, so we need to receive
            // several good pulses before the motors turn on.
            good_pulse_count[i] = 0;
        }
    }
}
