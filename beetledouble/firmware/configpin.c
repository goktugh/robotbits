#include <avr/io.h>

#define F_CPU 20000000 /* 20MHz / prescale=1 */
#include <util/delay.h>

#include <stdbool.h>
#include <stdint.h>

#include "diag.h"

/*
 * The config pin is shared with TXDEBUG on PA1.
 * 
 * This is achieved by using a high(ish) value resistor and a pull
 * down jumper link.
 * 
 * If the link is closed, then the pin is pulled down with a resistor,
 * but can still be driven strongly enough by the MCU that 
 * a) The TXDEBUG signal can still get out so we can see our diag
 * b) It does not draw too much current
 * 
 * 
 * We can detect the state of the link by setting the pin as an
 * INPUT, then using the ADC to read it. This would read a very low value
 * if the link is closed.
 * 
 * But if the link is open, it would be floating. So we can enable
 * internal pullup resistor on the MCU (temporarily) and check the ADC value.
 * 
 * We expect the internal pullup to pull the pin higher when the link is closed
 * than when the link is open.
 * 
 * The internal pullup has a nominal resistance of 35k ohm,
 * Min 20k max 50k
 * 
 * So if the ADC reports a voltage below about 80% of the supply,
 * we can reasonably assume that the link is closed, ignoring any
 * impedence of the device which might be connected to TXDEBUG.
 */
 
// Set to 0= low (short) or 1= high (open)
bool configpin_value;

#define CONFIG_PORT PORTA
#define CONFIG_PIN 1

// 10 bits, so max=1023
#define HIGH_THRESHOLD 550

void configpin_init()
{
    // Wait for any diag already printing to finish.
    _delay_ms(2);
    // Temporarily set the port as input.
    CONFIG_PORT.DIRCLR = (1 << CONFIG_PIN);
    // Enable pull up resistor
    CONFIG_PORT.PIN1CTRL = PORT_PULLUPEN_bm;
    // Wait for port to settle.
    _delay_ms(2);
    // Enable ADC0 - AIN1 
    ADC0.MUXPOS = 0x1; // AIN1
    // Set prescale to 32 to get best accuracy
    ADC0.CTRLC = 
        // REFSEL: use VDD as reference.
        (0x1 << 4) |
        // Clock prescale
        0x4 |
        // SAMPCAP enable (for higher voltages)
        ADC_SAMPCAP_bm;
    // Set enable bit
    ADC0.CTRLA = 0x1; 
    // Take some conversions...
    // delay a little to let the ADC settle.
    _delay_ms(10);
    // Run several conversions. They should all return about the same
    // result, so we ignore all except the last.
    for (uint8_t i=0; i<10; i++) {
        ADC0.COMMAND = 0x1; // Start conversion
        // This bit auto-clears when finished.
        while (ADC0.COMMAND & 0x1) {
            // Nothing
        }
    }
    
    // Now we can take the last 10-bit result.
    // This is expected to be very high ( >80%) if the link is open.
    uint16_t res = ADC0.RES;
    
    // Reset everything...
    ADC0.CTRLA = 0; // turn off adc
    CONFIG_PORT.PIN1CTRL = 0; // disable pullup
    // Make the pin output again
    CONFIG_PORT.DIRSET = (1 << CONFIG_PIN);
    // Delay a little to let the pin settle.
    _delay_ms(2);
    
    diag_println("configpin: ADC result %04x", res);
    bool is_high = (res >  HIGH_THRESHOLD);
    if (is_high) {
        diag_println("configpin: high (open)");
    } else {
        diag_println("configpin: low (closed)");
    }
    configpin_value = is_high;
}
