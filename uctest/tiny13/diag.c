#include <avr/io.h>
#include <avr/pgmspace.h>

#define F_CPU 9600000
#include <util/delay.h>

// Port to send diagnostic (serial) data:
#define PIN_DIAG 4

// Bit bang at this baud rate:
#define BIT_WAIT_TIME_MICROS ((1000L * 1000L) / 9600) 

static void bitbang_wait()
{
    _delay_us(BIT_WAIT_TIME_MICROS);
}

static void bitbang_char(unsigned char c)
{
    // Wait for one tick
    // Start bit should be low.
    const uint8_t diag_bit = 1 << PIN_DIAG;
    PORTB &= ~ diag_bit;
    bitbang_wait();
    // Loop through bits, lowest first.
    for (uint8_t i=0; i<8; i++) {
        if (c & 0x1) {
            PORTB |= diag_bit; // 1=high
        } else {
            PORTB &= ~diag_bit; // 0=low
        }
        c = c >> 1;
        bitbang_wait();
    }
    // Stop bit.
    PORTB |= diag_bit;
    bitbang_wait();
}

void diag_putc(unsigned char c)
{
    bitbang_char(c);
}

void diag_puts(const char *s)
{
    while (*s) {
        bitbang_char(*s);
        s += 1;
    }
}
void diag_puts_progmem(const char *s)
{
    do {
        char c = pgm_read_byte(s);
        if (c != 0) bitbang_char(c);
        else break;
        ++s;
    } while (1);
}


static char hexdigit(uint8_t n)
{
    n = n & 0xf;
    if (n < 10) {
        return n + '0';
    } else {
        return (n - 10) + 'A';
    }
}

void diag_printhex_8(uint8_t n)
{
    bitbang_char(hexdigit(n >> 4));
    bitbang_char(hexdigit(n));
}

void diag_printhex_16(uint16_t n)
{
    for (uint8_t i=0; i<4; i++) {
        bitbang_char(hexdigit(n >> (4*(3-i))));
    }
}

void diag_newline()
{
    diag_puts_progmem(PSTR("\r\n"));
}

void diag_init()
{
    DDRB |= (1 << PIN_DIAG); // Enable output on diagnostic pin
}
