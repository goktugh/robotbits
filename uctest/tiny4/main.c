/*
 * Test program for the avr attiny4 (or 5,9,10)
 * 
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

int main()
{
    init_clock();
    uint8_t blink_bit = 1 << 0;
    DDRB |= blink_bit; // Enable output on PB0 
   
    uint16_t counter=0; 
    while(1) {
        PORTB |= blink_bit;
        _delay_ms(250); 
        PORTB &= ~blink_bit;
        _delay_ms(250); 
        counter += 1;
    }
}
