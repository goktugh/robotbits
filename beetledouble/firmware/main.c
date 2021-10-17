#include <avr/io.h>

#define F_CPU 20000000 /* 20MHz / prescale=1 */
#include <util/delay.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

#include "diag.h"
#include "motors.h"
#include "rxin.h"
#include "isense.h"
#include "vsense.h"
#include "configpin.h"

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

int main(void)
{
    init_clock();
    init_serial();
    diag_puts("\r\n\nBeetledouble ESC starting\r\n");
    configpin_init();
    // We should initialise isense before the motors, because we do not
    // want to detect any spurious current charging the bst caps.
    // When calibrating the isense offset.
    isense_init();

    motors_init();
    rxin_init();
    while(1) {
        bool timer_overflow = motors_loop();
        rxin_loop();
        if (timer_overflow)
        {
			rxin_timer_overflow();
            vsense_timer_overflow();
            isense_timer_overflow();
        }
    }
}

/*

TODO 
1. Implement dead zone and centre braking (DONE)
1. Implement failsafe / rx timeout which will set motor ot zero
	zero power if no signal for a while (DONE)
1. Implement over current protection using the ADC and shunt
	- Overcurrent should cut out the motors (both) for a time
	- Regardless of inputs, overcurrent should override.

TESTING:
1. Test dead zone / failsafe
1. Test pulses too long / too short should not activate motor.
1. MOAR CURRENT testing
1. Check that the adc working by putting some current through
	the shunt.
1. Full test of overcurrent limit

 
 */
