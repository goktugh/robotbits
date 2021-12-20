#include <avr/io.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define F_CPU 20000000 /* 20MHz / prescale=1 */
#include <util/delay.h>
#include <avr/cpufunc.h>

// io pin
#define PORT_DIAG PORTA
#define PIN_DIAG 1

// Bit bang at this baud rate:
#define BAUD_RATE 115200
#define BIT_WAIT_TIME_MICROS ((1000L * 1000L) / BAUD_RATE) 

static void bitbang_wait()
{
    _delay_us(BIT_WAIT_TIME_MICROS);
    _NOP();
}

static void bitbang_char(unsigned char c)
{
    // Wait for one tick
    // Start bit should be low.
    const uint8_t diag_bit = 1 << PIN_DIAG;
    PORT_DIAG.OUTCLR = diag_bit;
    bitbang_wait();
    _NOP();
    // Loop through bits, lowest first.
    for (uint8_t i=0; i<8; i++) {
        if (c & 0x1) {
            PORT_DIAG.OUTSET = diag_bit; // 1=high
        } else {
            PORT_DIAG.OUTCLR = diag_bit; // 0=low
        }
        c = c >> 1;
        bitbang_wait();
    }
    // Stop bit.
    PORT_DIAG.OUTSET = diag_bit; // high
    bitbang_wait();
}

// Write a string to the port TXDEBUG through bit-bang
// This assumes the hardware is already configured.
void diag_puts(const char *str)
{
    PORT_DIAG.DIRSET = (1 << PIN_DIAG);
	const char *p=str;
	while (*p != 0) {
        bitbang_char(*p);
		++p ;
	}
}

#define DIAG_BUFSIZE 80

#ifdef ENABLE_DIAG
    static char buf[DIAG_BUFSIZE];

    void diag_println(const char * fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        buf[DIAG_BUFSIZE -1] = '\0'; // ensure null terminated
        diag_puts(buf);
        diag_puts("\r\n");
    }

    void diag_print(const char * fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        buf[DIAG_BUFSIZE -1] = '\0'; // ensure null terminated
        diag_puts(buf);
    }

#else

    void diag_println(const char * fmt, ...) {}
    void diag_print(const char * fmt, ...) {}

#endif 

void epic_fail(const char *msg) 
{
    diag_puts(msg);
    diag_puts("\r\n");
    // Ultra low power 32k internal oscilator
    uint8_t val = 0x1; // OSCULP32K
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, val);
    // Forever loop
    for ( ;; );
    
}
