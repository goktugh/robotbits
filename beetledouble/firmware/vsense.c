
#include "vsense.h"
#include "diag.h"

#include <avr/io.h>

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
 */ 
void vsense_init() 
{
    
}

void vsense_timer_overflow() 
{
    
}
