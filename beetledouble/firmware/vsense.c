
#include "vsense.h"
#include "diag.h"

#include <avr/io.h>
#define F_CPU 20000000 /* 20MHz / prescale=1 */
#include <util/delay.h>
#include <stddef.h>

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
 
#define VOLTAGE_MIN 9600
#define VOLTAGE_MAX 25500
 
bool vsense_ok;
static uint16_t last_voltage;

static void print_voltage();
static uint16_t calc_voltage();
 
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
    last_voltage = calc_voltage();
    print_voltage();
}

static uint16_t calc_voltage()
{
    uint16_t res = ADC0.RES;
    // Full range would give 2.5 volts
    // Full range is value 1023 / 0x3ff
    // Approx mv
    uint16_t sense_mv = (res * 5) /2;
    // Now we need to multiply by 23 to get the actual voltage.
    uint16_t mv = (sense_mv * 23);
    return mv;
}

static void print_voltage()
{
    diag_println("Bat. voltage = %4d", last_voltage);
}


#define BAD_VOLTAGE_TICKS 160

// Initially set bad_voltage_count to the maximum badness,
// then we will not turn on immediately if undervolt.

static uint8_t bad_voltage_count=BAD_VOLTAGE_TICKS; // Number of ticks we see bad voltage

static uint8_t info_count; // to limit how often we write debug msgs.

void vsense_timer_overflow() 
{
    // Called approximately every 26 milliseconds.
    
    last_voltage = calc_voltage();
    
    bool ok = (
        (last_voltage >= VOLTAGE_MIN) &&
        (last_voltage <= VOLTAGE_MAX)
        );
    
    
    if (ok) {
        bad_voltage_count = 0;
    } else {
        if (bad_voltage_count < BAD_VOLTAGE_TICKS) 
            bad_voltage_count += 1;
    }
    
    // Set vsense_ok if voltage has not been bad for very long.
    //
    vsense_ok = (bad_voltage_count < BAD_VOLTAGE_TICKS);
    if (! vsense_ok) {
        if (info_count == 0) {
            print_voltage();
            diag_println("vsense: voltage out of range.");
            info_count = 40;
        } else {
            info_count --;
        }
    }
}
