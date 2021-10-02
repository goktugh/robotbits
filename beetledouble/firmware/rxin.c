#include <avr/io.h>

#include "diag.h"
#include "motors.h"
#include <stdlib.h>

#define CHANNEL_COUNT 2

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
        // So it wraps after (65536 / 10) milliseconds 
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
                // Ignore.
                return;
            }
            
            // This is in units of 100ns,
            // we need to divide by 10 to get microseconds,
            /* This debug is handy but messes up motor drive */
            /*
            uint16_t pulsewidth_us = pulsewidth / 10;
            diag_println("p %c %04x", index + '0', pulsewidth_us);
            */
            // Let's find a more reasonable range,
            int16_t pulsewidth_signed = pulsewidth;
            pulsewidth_signed -= 15000; 
            // Now in the range approx -5000 to 5000
            uint16_t pulsewidth_abs = abs(pulsewidth_signed);
            // scale
            uint16_t pulsewidth_scaled = pulsewidth_abs / 16;
            // It is now in the range 0..312 (approx)
            // clamp
            if (pulsewidth_scaled > 255) pulsewidth_scaled = 255;
            motors_commands[index].duty = (uint8_t) pulsewidth_scaled;
            motors_commands[index].direction = (pulsewidth_signed > 0);
            // TODO: centre brake 
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
    
    check_timer(&TCB1, 0);
    check_timer(&TCB0, 1);
}
