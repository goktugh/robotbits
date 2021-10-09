#include <avr/io.h>

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
        USART_TXEN_bm | USART_RXEN_bm; // Start Transmitter and receiver
    // Enable interrupts from the usart rx
    // USART0.CTRLA |= USART_RXCIE_bm;
}

/*
 * Ports to test as output:
 * 
 * PA4,5,6,7
 * PB0,1 (NOT PB2 which is GNDSHUNT)
 */

static uint8_t bm_a = 0xf0; // 4,5,6,7
static uint8_t bm_b = 0x03; // 0,1;

static void all_outputs_low()
{
    PORTA.OUTCLR = bm_a;
    PORTA.DIRSET = bm_a;
    PORTB.OUTCLR = bm_b;
    PORTB.DIRSET = bm_b; 
}

// Make all output pins inputs, if that even makes sense?
static void all_pins_in()
{
    PORTA.DIRCLR = bm_a;
    PORTB.DIRCLR = bm_b; 
}

static void dump_port(PORT_t * port, char portname) {
    diag_print("INPUTS FOR PORT %c: ", portname);
    uint8_t value = port->IN; // read port
    for (int8_t i=7; i>=0; --i) {
        diag_print("%d", (value << i) & 1); 
    }
    diag_puts("\r\n");
}
static void dump_ports()
{
    dump_port(&PORTA, 'A');
    dump_port(&PORTB, 'B');
}

static void test_pin(PORT_t *port, char portname, uint8_t pin) 
{
    diag_println("Testing PORT %c PIN %d", portname, pin);
    all_outputs_low();
    _delay_ms(50);
    all_pins_in();
    // Set the pin low
    port->DIRSET = 1 << pin;
    _delay_ms(10);
    dump_ports();
    // Set the pin high
    port->OUTSET = 1 << pin;
    _delay_ms(10);
    dump_ports();
    diag_println("Done");
    all_pins_in();
}


static void test_pin_shorts()
{
    for (uint8_t i=4; i<8; i++) {
        test_pin(&PORTA, 'A', i);
    }
    for (uint8_t i=0; i<2; i++) {
        test_pin(&PORTB, 'B', i);
    }
}

static void go_to_sleep()
{
    // Ultra low power 32k internal oscilator
    uint8_t val = 0x1; // OSCULP32K
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, val);
    // Foreer loop
    for ( ;; );
    
}

int main(void)
{
    init_clock();
    init_serial();
    diag_puts("\n\nTest firmware starting\r\n");
    test_pin_shorts();
    go_to_sleep();
}

