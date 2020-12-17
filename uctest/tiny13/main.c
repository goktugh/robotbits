/*
 * Test program for the avr attiny13
 * 
 * Newer attiny are much better, e.g. attiny402 has more of everything
 * and is just as cheap.
 * 
 * But the attiny13 is widely available and jlc always have them in stock.
 */
#include "diag.h"

#include <avr/io.h>

// attiny13 maximum internal clock is nominally 9.6mhz
#define F_CPU 9600000
#include <util/delay.h>


static void dump_info()
{
    diag_puts("thing:");
    diag_printhex_8(42);
    diag_newline();
}

int main()
{
    uint8_t blink_bit = 1 << 0;
    DDRB |= blink_bit; // Enable output on PB0 
    
    diag_init();
    diag_puts("\r\nHello, world\r\n\r\n"); 
    dump_info();

    uint16_t counter = 0;
    while(1) {
        diag_puts("Loop ");
        diag_printhex_16(counter);
        diag_newline(); 
        PORTB |= blink_bit;
       _delay_ms(250); 
        PORTB &= ~blink_bit;
       _delay_ms(250); 
       counter += 1;
    }
}
