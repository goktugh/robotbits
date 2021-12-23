#include <avr/io.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "bitbang.h"
#include "diag.h"

// io pin
#define PORT_DIAG PORTA
#define PIN_DIAG 1

char diag_buf[DIAG_BUFSIZE];

static void bitbang_char(unsigned char c)
{
    bitbang_char_to_port(&PORT_DIAG, PIN_DIAG, c);
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

#ifdef ENABLE_DIAG

    void diag_println(const char * fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(diag_buf, sizeof(diag_buf), fmt, ap);
        va_end(ap);
        diag_buf[DIAG_BUFSIZE -1] = '\0'; // ensure null terminated
        diag_puts(diag_buf);
        diag_puts("\r\n");
    }

    void diag_print(const char * fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(diag_buf, sizeof(diag_buf), fmt, ap);
        va_end(ap);
        diag_buf[DIAG_BUFSIZE -1] = '\0'; // ensure null terminated
        diag_puts(diag_buf);
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
