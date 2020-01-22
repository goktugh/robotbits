#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 3333333 /* 20MHz / 6(default prescale) */
#include <util/delay.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "diag.h"

#include <stdlib.h>

static void init_hardware()
{
    USART0.CTRLC =
            USART_CMODE_ASYNCHRONOUS_gc | // Mode: Asynchronous[default]
            USART_PMODE_DISABLED_gc | // Parity: None[default]
            USART_SBMODE_1BIT_gc | // StopBit: 1bit[default]
            USART_CHSIZE_8BIT_gc; // CharacterSize: 8bit[default]
    uint32_t clk_per_hz = 3333L * 1000; // CLK_PER after prescaler in hz
    uint32_t want_baud_hz = 9600; // Baud rate
    uint16_t baud_param = (64 * clk_per_hz) / (16 * want_baud_hz);
    USART0.BAUD = baud_param;
    USART0.CTRLB = 
        USART_TXEN_bm; // Start Transmitter and receiver
    // Default pin for tx is PB2 / pin14 (on the qfn24 package)
    PORTB.DIRSET = 1 << 2; // Set pin as output
}


// Bit bang at this baud rate:
#define BIT_WAIT_TIME_MICROS ((1000L * 1000L) / 9600) 

static void bitbang_wait()
{
    _delay_us(BIT_WAIT_TIME_MICROS);
}

static void bitbang_char(PORT_t * port, uint8_t pin, unsigned char c)
{
    // Set port as output
    uint8_t pin_bm = 1 << pin;
    port->DIRSET = pin_bm;
    // Set it high
    port->OUTSET = pin_bm;
    bitbang_wait();
    // Wait for one tick
    // Start bit should be low.
    port->OUTCLR = pin_bm;
    bitbang_wait();
    // Loop through bits, lowest first.
    for (uint8_t i=0; i<8; i++) {
        if (c & 0x1) {
            port->OUTSET = pin_bm; // 1=high
        } else {
            port->OUTCLR = pin_bm; // 0
        }
        c = c >> 1;
        bitbang_wait();
    }
    // Stop bit.
    port->OUTSET = pin_bm;
    bitbang_wait();
}

#define BUFSIZE1 80

static char buf[BUFSIZE1];

static int bitbang_sprintf(PORT_t *port, uint8_t pin, const char * fmt, ...)
{
    va_list ap;
    int len;
    va_start(ap, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    buf[BUFSIZE1 -1] = '\0'; // ensure null terminated
    for (uint8_t i=0; i<len; i++) {
        bitbang_char(port, pin, (unsigned char) buf[i]);
    }
    return len;
}

static void test_ports()
{
    char pc = 'A';
    uint8_t pin;
    const char * msg = "This is PORT %c %d\r\n";
    // NB: PA0 is UDPI/RESET
    for (pin=1; pin<=7; pin++) {
        bitbang_sprintf(&PORTA, pin, msg, pc, (int) pin);
    }
    // NB: hardware uart is on PB2
    pc = 'B';
    for (pin=0; pin<=7; pin++) {
        if (pin != 2) {
            bitbang_sprintf(&PORTB, pin, msg, pc, (int) pin);
        }
    }
    pc = 'C';
    for (pin=0; pin<=7; pin++) {
        bitbang_sprintf(&PORTC, pin, msg, pc, (int) pin);
    }
}

static void mainloop()
{
    uint32_t counter = 0;
    PORTA.DIRSET = 1 << 5;
    while (1) {
        diag_println("Hardware USART is on PB2"); 
        diag_println("The main loop has executed %ld times", counter); 
        test_ports();
        _delay_ms(500);
        counter += 1;
    }
}

int main(void)
{
	init_hardware();	
	sei();  // enable interrupts
	diag_puts("\r\n\r\n");
	diag_puts("Hello, world");
	diag_puts("\r\n\r\n");
	mainloop();
}
