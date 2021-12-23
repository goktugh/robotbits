// Config interface for serial port

/*
 * We use the rxin1 pin as a bidirectional serial port.
 * 
 * The programmer should connect their serial port with a
 * 4.7k resistor on the tx pin.
 * 
 * 
 * The programmer sends a few CR characters (return/enter) to the 
 * port, without any other signals, 
 * 
 * 
 */

#include "configmode.h"
#include <avr/io.h>
#include "diag.h"
#include "motors.h"
#include "bitbang.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

// This flag - are we listening on the config port?
// We set this to FALSE as soon as we receive a good pwm signal,
// then disable the rx and don't worry again.
static uint8_t is_waiting;

static uint8_t cr_count; // Number of successive CRs

#define INPUT_BUF_LEN 10
static char input_buf[INPUT_BUF_LEN];
static uint8_t input_pos;

#define COMM_PORT PORTB
#define COMM_PIN 3

typedef struct {
    const char *name;
    uint8_t default_value;
    uint8_t min_value;
    uint8_t max_value;
} config_var_t;

const config_var_t all_config_vars[] = {    
    // Name, default, min, max
    {"Mixing (0=off 1=on, 2=depends on JP1)", 2, 0, 2},
    {"Mixing amount %", 60, 25, 100},
    {"Braking on", 1, 0, 1},
    {"Voltage cutoff 3S x0.1V", 96, 0, 120},
    {"Voltage cutoff 4S x0.1V", 128, 0, 160},
    {"Overcurrent limit (instantaneous) Amps", 25, 0, 35},
    {"Startup sound on",  1, 0, 1},
    {NULL, 0, 0, 0}
};

void configmode_init()
{
    // Use the USART for the receiver interface to detect bytes
    // from the PWMIN1 pin.
    uint32_t want_baud_hz = 115200; // Baud rate
    uint32_t clk_per_hz = F_CPU; // CLK_PER after prescaler in hz
    uint16_t baud_param = (64 * clk_per_hz) / (16 * want_baud_hz);
    USART0.BAUD = baud_param;
    USART0.CTRLB = 
        USART_RXEN_bm; // Start receiver   
    is_waiting = 1;
}

static void configmode_main();
static void comm_puts(const char *str);
static void mainloop();
static void show_config_vars();
static void comm_println(const char * fmt, ...);
static void comm_separator(char c);
static void flush_rx();

void configmode_timer_overflow()
{    
    // Called periodically; poll for byte received
    if (USART0.STATUS & USART_RXCIF_bm) {
        uint8_t d = USART0.RXDATAL;
        if (d == 0xd) {
            cr_count +=1;
            if (cr_count >= 4) {
                configmode_main(); // Never return
            }
        } else {
            cr_count = 0;
        }
    }
}

static void configmode_main()
{
    // In case the motors were about to do something, turn them off.
    motors_overcurrent_off();
    diag_puts("\r\nConfig mode activated (further messages on rx comm port)\r\n");
    comm_puts("\r\nTarakan ESC config mode\r\n");
    show_config_vars();
    flush_rx();
    while (1) {
        mainloop();
    }
}

static void show_config_vars()
{
    comm_separator('=');
    comm_println("Tarakan ESC config - current settings");
    comm_separator('=');
    comm_println("%8s %8s %s", "cmd", "value", "info");
    comm_separator('-');
    uint8_t a=0; // Parameter index
    while (all_config_vars[a].name != NULL)
    {
        char ch = 'A' + a;
        comm_println("%8c %8d %s", ch, 
            all_config_vars[a].default_value,  
            all_config_vars[a].name
            );
        a += 1;
    }
    comm_separator('=');
}

static void flush_rx()
{
    USART0.CTRLB = 0; // Stop receiver and flush.
    USART0.CTRLB = USART_RXEN_bm; // Start receiver   
}

static void handle_input_line()
{
    input_buf[INPUT_BUF_LEN - 1] = '\0';
    comm_println("Got input: [%s]", input_buf);
    
    // Clear old buf
    memset(input_buf,0,INPUT_BUF_LEN);
    input_pos = 0;
    // Clear data in the hardware receive buffer,
    // Because the connection is half duplex, anything we transmit
    // will also be received. 
    flush_rx();
}

static void mainloop()
{
    if (USART0.STATUS & USART_RXCIF_bm) {
        // Character input
        uint8_t d = USART0.RXDATAL;
        if (d == 0xd) {
            // CR
            handle_input_line();
        } else {
            input_buf[input_pos] = d;
            input_pos +=1 ;
            if (input_pos >= INPUT_BUF_LEN)
                input_pos = INPUT_BUF_LEN - 1;
        }
    }    
}

static void comm_separator(char c)
{
    const uint8_t len = 60;
    for (uint8_t i=0; i< len; i++) {
        diag_buf[i] = c;
    }
    diag_buf[len] = '\0';
    comm_puts(diag_buf);
    comm_puts("\r\n");
}

static void comm_puts(const char *str)
{
    COMM_PORT.DIRSET = (1 << COMM_PIN);
	const char *p=str;
	while (*p != 0) {
        bitbang_char_to_port(&COMM_PORT, COMM_PIN, *p);
		++p ;
	}
    bitbang_wait();
    COMM_PORT.DIRCLR = (1 << COMM_PIN);
}

static void comm_println(const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(diag_buf, sizeof(diag_buf), fmt, ap);
    va_end(ap);
    diag_buf[DIAG_BUFSIZE -1] = '\0'; // ensure null terminated
    comm_puts(diag_buf);
    comm_puts("\r\n");
}
