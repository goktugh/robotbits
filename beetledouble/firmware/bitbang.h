#pragma once

/*
 * This is a header-file include and should have no
 * non-static functions or variable definitions
 */

#include <avr/io.h>
#define F_CPU 20000000 /* 20MHz / prescale=1 */
#include <util/delay.h>
#include <avr/cpufunc.h>

// Bit bang at this baud rate:
#define BAUD_RATE 115200
#define BIT_WAIT_TIME_MICROS ((1000L * 1000L) / BAUD_RATE) 

static void bitbang_wait()
{
    _delay_us(BIT_WAIT_TIME_MICROS);
    _NOP();
}

static void bitbang_char_to_port(PORT_t *port, uint8_t pin, unsigned char c)
{
    // Wait for one tick
    // Start bit should be low.
    const uint8_t diag_bit = 1 << pin;
    port->OUTCLR = diag_bit;
    bitbang_wait();
    _NOP();
    // Loop through bits, lowest first.
    for (uint8_t i=0; i<8; i++) {
        if (c & 0x1) {
            port->OUTSET = diag_bit; // 1=high
        } else {
            port->OUTCLR = diag_bit; // 0=low
        }
        c = c >> 1;
        bitbang_wait();
    }
    // Stop bit.
    port->OUTSET = diag_bit; // high
    bitbang_wait();
}

