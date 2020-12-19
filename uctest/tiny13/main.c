/*
 * Test program for the avr attiny13
 * 
 * Newer attiny are much better, e.g. attiny402 has more of everything
 * and is just as cheap.
 * 
 * But the attiny13 is widely available and jlc always have them in stock.
 */
#include "diag.h"
#include "timer.h"

#include <avr/io.h>

// attiny13 maximum internal clock is nominally 9.6mhz
#define F_CPU 9600000
#include <util/delay.h>

#include <avr/boot.h>
#include <avr/pgmspace.h>

static void dump_info()
{
    diag_puts_progmem(PSTR("fuse hi:"));
    diag_printhex_8(boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS));
    diag_newline();
    diag_puts_progmem(PSTR("fuse lo:"));
    diag_printhex_8(boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS));
    diag_newline();
    
}

static const char greeting[] PROGMEM = "\r\nHello, world\r\n\r\n";

static void dump_current_time()
{
    uint32_t now = timer_read();
    diag_printhex_16(now >> 16);
    diag_printhex_16(now);
}

static void test_timer()
{
    diag_puts_progmem(PSTR("test_timer\r\n"));
    for (uint8_t i=0; i<20; i++) {
        dump_current_time();
        diag_newline();    
    }
    for (uint8_t i=0; i<10; i++) {
        timer_wait(10000);
        diag_putc('0' + i);
    }
    diag_newline();
}

int main()
{
    uint8_t blink_bit = 1 << 0;
    DDRB |= blink_bit; // Enable output on PB0 
    
    diag_init();
    diag_puts_progmem(greeting); 
    timer_init();
    test_timer();
    dump_info();

    uint16_t counter = 0;
    while(1) {
        diag_puts("Loop ");
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
