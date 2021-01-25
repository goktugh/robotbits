/*
 * Beetleback ESC.
 * 
 * For attiny13a.
 * 
 * The attiny13 is widely available and jlc always have them in stock.
 */
#include "diag.h"
#include "timer.h"
#include "defs.h"

#include <avr/io.h>

#include <util/delay.h>

#include <avr/boot.h>
#include <avr/pgmspace.h>

static void dump_info()
{
    // We could print any info we want here.    
}

static const char greeting[] PROGMEM = "\r\nBeetleback\r\n\r\n";

static void dump_current_time()
{
    uint32_t now = timer_read();
    diag_printhex_16(now >> 16);
    diag_printhex_16(now);
}

static void clock_init()
{
    // Program clock prescaler. This is initially set from a fuse
    // but we can change it at runtime.
    // 0x0 = 1 (no scaling) 0x1 = 2 (divide by 2)
    CLKPR = (1 << CLKPCE); // We now have 4 cycles to write the value.
    CLKPR = 0x1;  
}

int main()
{
    clock_init();
    
    uint8_t blink_bit = 1 << 4;
    DDRB |= blink_bit; // Enable output on blinky led
    
    diag_init();
    diag_puts_progmem(greeting); 
    timer_init();
    dump_info();

    uint16_t counter = 0;
    while(1) {
        diag_puts_progmem(PSTR("Loop "));
        diag_printhex_16(counter);
        diag_putc(' ');
        dump_current_time();
        diag_newline(); 
        PORTB |= blink_bit;
        timer_wait_ms(250); 
        PORTB &= ~blink_bit;
        timer_wait_ms(250); 
        counter += 1;
    }
}
