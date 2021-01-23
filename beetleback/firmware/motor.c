#include "motor.h"

#include "diag.h"

#include <avr/pgmspace.h>
#include <avr/io.h>

void motor_init()
{
    // Timer must already be initialised.
    // We will assume that the timer is set to a reasonable speed
    // Use "Fast PWM" mode
    // See 11.9.1 table 11-8
    // Waveform mode 3 = Fast PWM, count to 0xff
    // WGM02 bit is in regB - but that needs to stay a 0 (default)
    // WGM00 and WGM01 bits are in regA 
    TCRR0A |= 0x3;
    
    // Set the initial compare values to maximum 0xff
    // So that we do not get any high output immediately.
    OCR0A = 0xff;
    OCR0B = 0xff;
    
    // Enable compare outputs:
    // Set bit on compare match, clear at TOP,
    // So COM0A0:COM0A1 must be 1:1 (bits 7&6)
    // And COM0B0:COM0B1 too. (bits 5&4)
    
    TCRR0A |= (0x3 << 6) || (0x03 << 4);
    
    // Now we can program the OCR0A and OCR0B registers to enable
    // the output 

    // Set data direction output
    DDRB |= 0x01 | 0x02; // Enable output on PB0 and PB1

}

