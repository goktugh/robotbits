#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 20000000 /* 20MHz / prescale=1 */
#include <util/delay.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

#include "diag.h"

static void init_clock()
{
    // This is where we change the cpu clock if required.
    // uint8_t val = CLKCTRL_PDIV_2X_gc | 0x1;  // 0x1 = PEN enable prescaler.
    uint8_t val = 0;  // 0 = no prescaler, full speed.
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, val);
    _delay_ms(10);
}

static void init_serial()
{
    // UART0- need to use "alternate" pins 
    // This puts TxD and RxD on PA1 and PA2
    PORTMUX.CTRLB = PORTMUX_USART0_ALTERNATE_gc; 
    // Diagnostic uart output       
    // TxD pin PA1 is used for diag, should be an output
    // And set it initially high
    PORTA.OUTSET = 1 << 1;
    PORTA.DIRSET = 1 << 1;
    
    uint32_t want_baud_hz = 230400; // Baud rate (was 115200)
    uint32_t clk_per_hz = F_CPU; // CLK_PER after prescaler in hz
    uint16_t baud_param = (64 * clk_per_hz) / (16 * want_baud_hz);
    USART0.BAUD = baud_param;
    USART0.CTRLB = 
        USART_TXEN_bm; // Start Transmitter
    // Enable interrupts from the usart rx
    // USART0.CTRLA |= USART_RXCIE_bm;
}

int main(void)
{
    init_clock();
    init_serial();
    diag_puts("\r\n\nBeetledouble diagnostic\r\n");
    
    while(1) {
        _delay_ms(500);
        diag_puts("Still working\r\n");
    }
}
