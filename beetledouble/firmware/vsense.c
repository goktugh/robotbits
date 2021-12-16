
#include "vsense.h"
#include "diag.h"

#include <avr/io.h>
#define F_CPU 20000000 /* 20MHz / prescale=1 */
#include <util/delay.h>

/*
 * Hardware revision 3
 * 
 * Read PB0 through the ADC, which is connected to a voltage divider
 * 
 * ratio 23:1 (1k resistor down, 22k up)
 * 
 * So we will usually get less than 1 volt.
 * 
 * 
 * Set voltage reference to 1.5v 
 * Then if we see 1.5v we will have 1.5*23 = 34 volts - too many
 * 
 * MAXIMUM VOLTAGE NOMINAL is 25v
 * So we will do nothing if >25.5v
 * 
 * MINIMUM VOLTAGE NOMINAL is 3S pack, 3*3.2 =~ approx 9.6 volts
 * 
 * If we reach 9.6 volts then we have too litt.e
 * 
 * If we start up with < 9.6 volts, the pack is dead or something, or
 * we are running on a bench supply or incorrect supply. do not drive
 * outputs.
 * 
 * NB: Use a different ADC from isense.c, 
 * 
 * ADC: ADC0
 * 
 */ 
 
static void print_voltage();
 
void vsense_init() 
{
    diag_println("vsense_init");
    // Disable digital input PB0
    PORTB.DIRCLR = 1 << 0; // Set port as input (should be unnecessary)
    PORTB.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
    
    // Configure adc
    ADC0.MUXPOS = 11; // PB0 = AIN11    
    ADC0.CTRLC = 
        // REFSEL: use internal reference.
        0 |
        // Clock prescale
        0x4 |
        // SAMPCAP enable (for higher voltages)
        ADC_SAMPCAP_bm;
    // Set voltage reference to 2.5 volts
    VREF.CTRLA = VREF_ADC0REFSEL_2V5_gc;
    // Set enable bit and auto 
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_FREERUN_bm;
    ADC0.COMMAND = 0x1; // Start conversion
    // Take some conversions...
    // delay a little to let the ADC settle.
    _delay_ms(10);
    print_voltage();
}

static void print_voltage()
{
    uint16_t res = ADC0.RES;
    // Full range would give 2.5 volts
    // Full range is value 1023 / 0x3ff
    // Approx mv
    uint16_t sense_mv = (res * 5) /2;
    // Now we need to multiply by 23 to get the actual voltage.
    uint16_t mv = (sense_mv * 23);
    diag_println("Bat. voltage = %4d", mv);
}

void vsense_timer_overflow() 
{
    
}
