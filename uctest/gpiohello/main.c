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
    uint32_t want_baud_hz = 115200; // Baud rate
    uint16_t baud_param = (64 * clk_per_hz) / (16 * want_baud_hz);
    USART0.BAUD = baud_param;
    USART0.CTRLB = 
        USART_TXEN_bm; // Start Transmitter and receiver
    // Default pin for tx is PB2 / pin14 (on the qfn24 package)
    PORTB.DIRSET = 1 << 2; // Set pin as output
}

static void mainloop()
{
    uint32_t counter = 0;
    PORTA.DIRSET = 1 << 5;
    while (1) {
        diag_println("The main loop has executed %ld times", counter); 
        counter += 1;
        PORTA.OUTSET = 1 << 5;
        _delay_ms(250);
        PORTA.OUTCLR = 1 << 5;
        _delay_ms(250);
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
