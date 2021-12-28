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
#include <ctype.h>
#include "configvars.h"
#include "configpin.h"
#include "vsense.h"

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
static void show_config_vars(bool defaults);
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

#define SYSTEM_NAME "Tarakan ESC"

static void configmode_main()
{
    // In case the motors were about to do something, turn them off.
    motors_overcurrent_off();
    diag_puts("\r\nConfig mode activated (further messages on rx comm port)\r\n");
    comm_puts("\r\n" SYSTEM_NAME " config mode\r\n");
    comm_println("Enter a parameter followed by a value e.g. A0 or ? for more help");
    show_config_vars(false);
    flush_rx();
    while (1) {
        mainloop();
    }
}

static void show_extra_info()
{
    if (configpin_value) {
        comm_println("JP1 state: open; mixing enabled if A=2"); 
    } else {
        comm_println("JP1 state: closed; mixing disabled if A=2");         
    }
    comm_println("Battery voltage: %d mV", vsense_last_voltage);
}

static void show_config_vars(bool defaults)
{
    comm_separator('=');
    if (defaults) {
        comm_println(SYSTEM_NAME " config - default settings");
    } else {
        comm_println(SYSTEM_NAME " config - current settings");       
        show_extra_info(); 
    }
    comm_separator('=');
    comm_println("%8s %8s %s", "cmd", "value", "info");
    comm_separator('-');
    uint8_t a=0; // Parameter index
    while (all_config_vars[a].name != NULL)
    {
        char ch = 'A' + a;
        uint8_t val;
        if (defaults) {
            val = all_config_vars[a].default_value;
        } else {
            val = *(all_config_vars[a].var_ptr);
        }
        comm_println("%8c %8d %s", ch, 
            val,  
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

static void show_error(const char *s)
{
    comm_println("E: %s", s);
}

static void show_unknown_command(char c)
{
    comm_println("E: Unknown command %c", c);
}

static void show_help()
{
    static const char *info[] = {
        "Enter a configuration variable and value, e.g. A0 or these commands:",
        "? = this help, W = show current values",
        "X = show default values without changing current values",
        "Y = reset all parameters to defaults",
        "Z = restart ESC (leave config mode)",
        NULL
    };
    for (uint8_t i=0; info[i]; i++) {
        comm_println(": %s", info[i]);
    }    
}

static void handle_other_commands(char cmd)
{
    switch (cmd) {
        case '?':
            show_help();
            break;
        case 'W':
            show_config_vars(false);
            break;
        case 'X':
            show_config_vars(true);
            break;
        case 'Y':
            comm_println("I: resetting all parameters to defaults");
            configvars_load_defaults();
            configvars_save();
            break;
        case 'Z':
            comm_println("I: resetting device");
            trigger_reset();
            break;
    }
}

static void handle_command(char cmd, int arg)
{
    // Convert command to upper case for ease of use.
    cmd = toupper(cmd);
    uint8_t cmd_index = cmd - 'A'; // May underflow
    if ((cmd == '?') || ((cmd >='W') && (cmd <= 'Z')) ) {
        handle_other_commands(cmd);
        return;
    }
    if ((cmd < 'A') || (cmd_index >= configvars_count())) {
        show_unknown_command(cmd);
        return;
    }
    // Check if the value is in range.
    bool inrange = ((arg >= 0) && arg <= 255);
    uint8_t minval = all_config_vars[cmd_index].min_value;
    uint8_t maxval = all_config_vars[cmd_index].max_value;
    if (inrange) {
        uint8_t arg_8 = arg;
        inrange = ((arg_8 >= minval) && (arg_8 <= maxval));
    }
    if (! inrange) 
    {
        comm_println("I: Parameter %c must be between %d and %d inclusive",
            cmd, minval, maxval);
        show_error("Parameter out of range");
        return;
    }
    
    // Set the parameter current value
    *(all_config_vars[cmd_index].var_ptr) = (uint8_t) arg;
    // Save to eeprom
    configvars_save();

    comm_println("OK: %c set to value %d", cmd, arg);
}

static void handle_input_line()
{
    char cmd = input_buf[0];
    input_buf[INPUT_BUF_LEN - 1] = '\0';
    int len = strlen(input_buf);
    if (len > 0) { // Ignore empty command
        comm_println("I: Command received: [%s]", input_buf);
        // Parse command argument
        char *endp = &(input_buf[1]);
        int arg = strtol(endp, &endp, 10);
        if (*endp != '\0') // Indicates error
        {
            show_error("Syntax error");
        } else {
            handle_command(cmd, arg);
        }
    }
    
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
    } else {
        if (USART0.STATUS & USART_BDF_bm) {
            // break detected
            trigger_reset();
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
