
#include "isense.h"
#include "diag.h"

#include <avr/io.h>

#define F_CPU 20000000 /* 20MHz / prescale=1 */
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 * Rev3 hardware we will need to change to use ADC1 peripheral.
 * 
 * ADC: ADC1
 * PIN: PB7
 * Channel: AIN3
 * 
 * Current sensor has 2x 10mOhm resistor in parallel = 5 mOhm.
 * 
 * So it will give 5mV per amp.
 * 
 * at 20 amps that means 0.1V which is plenty.
 * 
 * ADC - use 0.55V voltage reference for maximum sensitity.
* 
 */


volatile uint8_t overcurrent_time;

static uint16_t adc_zero_offset;
static bool isense_active;

#define SAMPLES_ACCUMULATED 32

static void show_current()
{
    uint16_t res = ADC1.RES;
    // shunt voltage = res / 1024 * 0.55V
    // Current will be shunt voltage / 5mV
    // This gives us current in 0.1 amp
    // 
    uint16_t res_relative = res - adc_zero_offset;
    if (res < adc_zero_offset) res_relative = 0;
    uint16_t res1 = res_relative / SAMPLES_ACCUMULATED;
    // 6 appox = 600mA
    uint16_t deciamp = res1;    
    diag_println("isense: %04x, %03d00 mA", res,deciamp);
}


/*
 * Initialise the threshold feature which will cut off 
 * overcurrent
 */
static void init_threshold()
{
    if (isense_active) {
        // Set up the threshold
        // TODO: Calculate correctly
        uint16_t threshold_amps = 3;
        // Units are approx 100mA
        uint16_t threshold = adc_zero_offset + (SAMPLES_ACCUMULATED * threshold_amps * 10);
        diag_println("isense threshold %04x (%d amps)", threshold, threshold_amps);
        ADC1.WINHT = threshold;
        ADC1.CTRLE = 0x2; // ABOVE threshold
        ADC1.INTCTRL = 1 << 1; // Enable WCMP interrupt
    }
}

ISR(ADC1_WCOMP_vect)
{
    // Threshold exceed. Overcurrent.
    // Number of ticks to turn off motors
    overcurrent_time = 10;
    ADC1.INTFLAGS = 0x02; // reset WCMP flag
}

static int uint16_compare(const void *a, const void *b)
{
    uint16_t va = *((uint16_t *) a);
    uint16_t vb = *((uint16_t *) b);
    if (va < vb) return -1;
    if (vb < va) return 1;
    return 0;
}

#define OFFSET_SAMPLES 9
static void init_offset()
{
    // Store the initial (offset) result

    uint16_t samples[OFFSET_SAMPLES ];
    adc_zero_offset = ADC1.RES; // Temporary value
    for(uint8_t i=0; i<OFFSET_SAMPLES; i++) {
        samples[i] = ADC1.RES;
        _delay_ms(1);
    }
    // Sort the results into numerical order
    qsort(samples, OFFSET_SAMPLES, sizeof(uint16_t),
        uint16_compare);
    for(uint8_t i=0; i<OFFSET_SAMPLES; i++) {
        diag_println("sample %04x", samples[i]);
    }
    
    // So we can use the median

    adc_zero_offset = samples[4];

    diag_println("isense_init: zero offset=%04x", adc_zero_offset);
    
    // What is the expected value?
    // Approximately 1/23 of the 5v rail or about 
    // 217mV
    // Full range is 550mV
    // Expected value will be * SAMPLES_ACCUMULATED
    uint16_t expected_value = ((217l * 1023) / 550) * SAMPLES_ACCUMULATED;
    diag_println("isense_init: expected value about %04x", expected_value);
    // Check adc_zero_offset is not rediculous.
    isense_active = (adc_zero_offset < 0x5000);
}

void isense_init() 
{
    // Set pin as input and disable digital input
    PORTA.DIRCLR = 1<<2;
    PORTA.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
    // Set up voltage reference
    // (VREF CTRLC bits 4-6 -> ADC1)
    VREF.CTRLC = 0x0 << 4; // 0.55 volts
    ADC1.MUXPOS = 0x3; // AIN3

    // Clock needs to be below 1.5mhz
    ADC1.CTRLC = 
        // REFSEL: use INTERNAL (VREF) as reference.
        (0 << 4) |    
        // Clock prescale
        0x3; // divide 16 = 1.25 mhz
        
    // Number of samples to accumulate
    // Must be the same as SAMPLES_ACCUMULATED
    ADC1.CTRLB = 0x5; // 0x5 = 32    samples
    // ADC timing = 13 cycles * CLK_ADC * number of samples
    // 0.8 us * 13 * 32 = 332.8 microseconds

    // Set enable bit and auto 
    ADC1.CTRLA = ADC_ENABLE_bm | ADC_FREERUN_bm;
    _delay_ms(10);  
    // Start first conversion
    ADC1.COMMAND = 0x1; // Start conversion
    // Wait for it to get going...
    _delay_ms(50);  
    diag_println("isense_init");

    init_offset();
    init_threshold();
    if (! isense_active) {
        diag_println("isense: disabled because sensor is giving bad values");
    }
}

static uint8_t ticks;

void isense_timer_overflow() 
{
    if (isense_active) 
    {
        bool show=false;
        if (overcurrent_time >0) {
            diag_puts("Overcurrent\r\n");
            overcurrent_time -= 1;
            show = true;
        }
        // Periodically print current
        if (ticks < 5) {
            ticks += 1;
        } else {
            if (show) show_current();
            ticks = 0;
        }
    }
}
