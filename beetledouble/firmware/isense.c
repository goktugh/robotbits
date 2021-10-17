
#include "isense.h"
#include "diag.h"

#include <avr/io.h>

#define F_CPU 20000000 /* 20MHz / prescale=1 */
#include <util/delay.h>

/*
 * NB: Rev3 hardware we will need to change to use ADC1 peripheral.
 * 
 * 
 * 
 * Current sensor has 2x 10mOhm resistor in parallel = 5 mOhm.
 * 
 * So it will give 5mV per amp.
 * 
 * at 20 amps that means 0.1V which is plenty.
 * 
 * ADC - use 0.55V voltage reference for maximum sensitity.
 * 
 * Rev2 hardware: Pin PA2
 *  ADC0 AIN2
 * 
 */


static uint16_t adc_zero_offset;
 
static void show_current()
{
    uint16_t res = ADC0.RES;
    // shunt voltage = res / 1024 * 0.55V
    // Current will be shunt voltage / 5mV
    // This gives us current in 0.1 amp
    // 
    uint16_t res_relative = res - adc_zero_offset;
    uint16_t deciamp = (res_relative * 7) / 64;    
    diag_println("isense: %03d00 mA", deciamp);
}

void isense_init() 
{
    // Set pin as input and disable digital input
    PORTA.DIRCLR = 1<<2;
    PORTA.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
    // Set up voltage reference
    VREF.CTRLA = 0x0 << 4; // 0.55 volts
    ADC0.MUXPOS = 0x2; // AIN2

    // Clock needs to be below 1.5mhz
    ADC0.CTRLC = 
        // REFSEL: use INTERNAL (VREF) as reference.
        (0 << 4) |    
        // Clock prescale
        0x4; // divide 32 = 0.625 mhz
        
    // Number of samples to accumulate
    ADC0.CTRLB = 0x6; // 0x6 = 64    samples

    // Set enable bit and auto 
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_FREERUN_bm;
    _delay_ms(10);  
    // Start first conversion
    ADC0.COMMAND = 0x1; // Start conversion
    // Wait for it to get going...
    _delay_ms(50);  
    diag_println("isense_init");

    // Store the initial (offset) result
    uint16_t res = ADC0.RES;
    adc_zero_offset = res;
    show_current();
}

static uint8_t ticks;

void isense_timer_overflow() 
{
    // Periodically pirnt current
    if (ticks < 20) {
        ticks += 1;
    } else {
        show_current();
        ticks = 0;
    }
}
