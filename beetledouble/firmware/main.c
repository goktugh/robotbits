#include <avr/io.h>
#include <avr/interrupt.h>

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
#include "configmode.h"
#include "configvars.h"

static void init_clock()
{
    // This is where we change the cpu clock if required.
    // uint8_t val = CLKCTRL_PDIV_2X_gc | 0x1;  // 0x1 = PEN enable prescaler.
    uint8_t val = 0;  // 0 = no prescaler, full speed.
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, val);
    _delay_ms(10);
}

int main(void)
{
    init_clock();
    configmode_init();
    diag_puts("\r\n\nBeetledouble ESC starting\r\n");
    // We should read the configpin early, before we use the ADCs
    // for anything else.
    configpin_init();
    // Load config variables from eeprom
    configvars_load();
    
    vsense_init(); // vsense uses the same ADC as configpin.
    // We should initialise isense before the motors, because we do not
    // want to detect any spurious current charging the bst caps.
    // When calibrating the isense offset.
    isense_init();

    motors_init();
    rxin_init();
    sei(); // interrupts on

    while(1) {
        bool timer_overflow = motors_loop();
        rxin_loop();
        if (timer_overflow)
        {
			rxin_timer_overflow();
            vsense_timer_overflow();
            isense_timer_overflow();
            configmode_timer_overflow();
        }
    }
}

/*

TODO 
1. flexible rxin - needs to loop through different
    receiver modes (pwm, sbus, ibus, config terminal) at startup
    * When valid signal is received, then assume config.
    * Only listen on PWMIN1 at startup.

1. Config interface 
*  If receiving some CR characters at expected baud rate,
        Assume config mode.
        Suspend normal esc operation and run only in config mode
        * until reboot.
        * 
1. SBUS / IBUS
* If those signals are detected, switch to mode.

1. MIXING
* When we are running normally, we should mix.
* Mixing - config on by default, optionally disable, using configpin,
    OR override config in eeprom.
* If mixing is enabled - require BOTH signals to be active to drive.
* If mixing is disabled, then signals should work independently

1. EEPROM CONFIG
* Low voltage cutoff
* Current limit / overcurrent protection settings
* Mixing overrides
* Startup delay (in case 2x ESC in the same robot, so their beeps do not
        happen at the same time)

1. Startup beep / tune
* Do a tune with a few notes on motor1 and motor2
* Arpeggio or familiar tune?


TESTING:
1. Test dead zone / failsafe
1. Test pulses too long / too short should not activate motor.
1. MOAR CURRENT testing
1. Check that the adc working by putting some current through
	the shunt.
1. Full test of overcurrent limit
    Can it survive a dead short?
 
 */
