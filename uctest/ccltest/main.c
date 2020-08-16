#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 10000000 /* 10mhz */
#include <util/delay.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "diag.h"

#include <stdlib.h>

static void init_clock()
{
    // This is where we change the cpu clock if required
    // 10mhz is probably the highest speed we can run at 3.3 volts.
    // internal oscillator goes at 20mhz by default, so we need to divide by 2.
    uint8_t val = CLKCTRL_PDIV_2X_gc | 0x1;  // 0x1 = PEN enable prescaler.
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, val);
    _delay_ms(10);
}

static void init_usart()
{
    // For debug out
    USART0.CTRLC =
            USART_CMODE_ASYNCHRONOUS_gc | // Mode: Asynchronous[default]
            USART_PMODE_DISABLED_gc | // Parity: None[default]
            USART_SBMODE_1BIT_gc | // StopBit: 1bit[default]
            USART_CHSIZE_8BIT_gc; // CharacterSize: 8bit[default]
    uint32_t clk_per_hz = 10000L * 1000; // CLK_PER after prescaler in hz
    uint32_t want_baud_hz = 9600; // Baud rate
    uint16_t baud_param = (64 * clk_per_hz) / (16 * want_baud_hz);
    USART0.BAUD = baud_param;
    USART0.CTRLB = 
        USART_TXEN_bm; // Start Transmitter and receiver
    // Default pin for tx is PB2 / pin14 (on the qfn24 package)
    PORTB.DIRSET = 1 << 2; // Set pin as output
}

static void init_timer()
{
    // Enable outputs on the pins
    // default PA4 and PA5
    PORTA.DIRSET = (1 << 4) | (1 << 5);
    
    // one ramp mode
    // enable WOA
    // enable WOB
    // Enable A and B comparator
    // And enable WOC and WOD
    TCD0.FAULTCTRL = 
        TCD_CMPAEN_bm   
        | TCD_CMPBEN_bm;
    _PROTECTED_WRITE(TCD0.FAULTCTRL,
        TCD_CMPAEN_bm | TCD_CMPBEN_bm | TCD_CMPCEN_bm | TCD_CMPDEN_bm);
 
    // Set the comparators up
    TCD0.CMPASET = 312; // Time before cmpa goes high
    TCD0.CMPBSET = 624; // Time before cmpb goes high
    // This is a 12-bit counter so it only counts to 4096.
    TCD0.CMPACLR = 4000; // reset time
    TCD0.CMPBCLR = 4000; // reset time
    /* ensure ENRDY bit is set */
    while(!(TCD0.STATUS & TCD_ENRDY_bm)) { };
    
    // Finally turn the thing on.
    TCD0.CTRLA = TCD_ENABLE_bm |
        TCD_CLKSEL_SYSCLK_gc | // sys clock
        TCD_SYNCPRES_DIV2_gc | // prescale by 2
        TCD_CNTPRES_DIV32_gc; // further prescale by 32
        
    // Enable PC0 as output for TCD.WOC
    PORTC.DIRSET |= 1 << 0;
    
}

static void init_ccl()
{
    // Set alternate output pins for ccl
    // PB4 pin12 + PC1 pin18
    PORTMUX.CTRLA |= PORTMUX_LUT0_ALTERNATE_gc | PORTMUX_LUT1_ALTERNATE_gc;
    // Set those pins as out
    PORTB.DIRSET |= 1 << 4;
    PORTC.DIRSET |= 1 << 1;
    // Set up LUT0
    // Truth:
    // Wire as a "NOR" so we or all the inputs together and NOT
    // So we set all bits to 0, except bit0
    CCL.TRUTH0 = 0x1;
    // Inputs: 
    // CCL.LUT0
    CCL.LUT0CTRLB = 0x9; // INSEL0 = 0x9 = TCD WOA bit
    // ALl other inputs will default to "mask" which I think is always 0
    CCL.LUT0CTRLA =  CCL_OUTEN_bm | CCL_ENABLE_bm; //enable out+lut

    // Set up LUT1
    CCL.TRUTH1 = 0x1;
    // Inputs: 
    // CCL.LUT1
    CCL.LUT1CTRLB = 0x9  << 4; // INSEL1 = 0x9 = TCD WOB bit
    // ALl other inputs will default to "mask" which I think is always 0
    CCL.LUT1CTRLA =  CCL_OUTEN_bm | CCL_ENABLE_bm; //enable out+lut

    CCL.CTRLA = CCL_ENABLE_bm; // Enable CCL
}

static void mainloop()
{
    uint16_t pulselen = 312;
    for (;;) {
        _delay_ms(10);
        // Switch pulse len
        TCD0.CMPBSET = pulselen;
        // synchronise at end of cycle
        // TCD0.CTRLE = TCD_SYNCEOC_bm;
        TCD0.CTRLE = TCD_SYNCEOC_bm;
        pulselen += 1;
        if (pulselen > 624) pulselen = 312;
    }
}

int main(void)
{
    init_clock();
	init_usart();
    init_timer();
    init_ccl();
	sei();  // enable interrupts
	diag_puts("\r\n\r\n");
	diag_puts("Hello, world");
	diag_puts("\r\n\r\n");
	mainloop();
}
