#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#define F_CPU 20000000 /* 10MHz / prescale=2 */
#include <util/delay.h>

#include "diag.h"
#include "motors.h"
#include "isense.h"
#include "vsense.h"
#include "configvars.h"

motor_command_t motors_commands[MOTORS_COUNT];

static uint16_t overflow_count;

/*
 * PB0 - MOTOR_ENABLE_1 - Active high enables the gates on M1
    Also connected to LED.
PA4 - MOTORF_1 - drives side 1 of M1 high
PA5 - MOTORR_1 - drives side 2 of M2 high



PB1 - MOTOR_ENABLE_2 - Active high enables the gates on M2
PA6 - MOTORF_2
PA7 - MOTORR_2
*/

// Enable pins:

#define PORT_MOTOR_ENABLE_1 PORTB
#define PIN_MOTOR_ENABLE_1 2

#define PORT_MOTOR_ENABLE_2 PORTB
#define PIN_MOTOR_ENABLE_2 1

// Gate drive pins

#define PORT_MOTOR PORTA

#define PIN_MOTOR1F 4   
#define PIN_MOTOR1R 5   

#define PIN_MOTOR2F 6
#define PIN_MOTOR2R 2

#define BITMAP_ALL_MOTORS ( \
		(1<< PIN_MOTOR1F) | \
		(1<< PIN_MOTOR1R) | \
		(1<< PIN_MOTOR2F) | \
		(1<< PIN_MOTOR2R)   \
)

/*
 * 
 * Note on charging bootstrap capactiors:
 * 
 * Bootstrap capacitors can only be charged when the enable pin is high
 * and the drive pin is low - driving the LOW side of each pin.
 * 
 * If the caps are not charged they will not have enough voltage to turn
 * the mosfet high sides on fully, which could cause troubel.
 */

#define TCA_PERIOD (32768)

#define RECHARGE_TIME_US (40)

// PWM_PERIOD should be a divisor of TCA_PERIOD.
// this is the period of our duty cycle pulses.
// Could be a power of two for faster divide
#define PWM_PERIOD (2048)

static void motors_init_timer()
{
	/*
	 * We use TCA in the simplest mode possible with no output generation
	 * and no interrupts, just poll its counts.
	 */
	// TCA - divide by 16 diving a period of 0.8us
	// Count up to TCA_PERIOD giving a wraparound time of
	// about 26ms
	// Clock selector: divide by 16.
	TCA0.SINGLE.PER = TCA_PERIOD;	
	TCA0.SINGLE.CTRLA = (0x04 << 1) |
		1 ; // Enable bit
}

/*
 * Note we only do this either at boot time or after
 * recovery from overcurrent.
 * 
 * This would also brake both motors but in overcurrent condition they
 * are probably stopped anyway.
 */
static void motors_all_low()
{
    // set all the half bridges to low
	PORT_MOTOR.OUTCLR = BITMAP_ALL_MOTORS;
    // Enable it all
	PORT_MOTOR_ENABLE_1.OUTSET = (1<< PIN_MOTOR_ENABLE_1);
	PORT_MOTOR_ENABLE_2.OUTSET = (1<< PIN_MOTOR_ENABLE_2);
}

void motors_early_init()
{
    // SO that the outputs are not floating, or
    // If we had a bootloader or updi or something which left the pins
    // in a weird state,
    // Turn everything off as soon as possible. 
	// Make enable pins outputs.
	PORT_MOTOR_ENABLE_1.DIRSET = (1<< PIN_MOTOR_ENABLE_1); 
	PORT_MOTOR_ENABLE_2.DIRSET = (1<< PIN_MOTOR_ENABLE_2);
	
	// Make the gate drive pins outputs.
	PORT_MOTOR.DIRSET = BITMAP_ALL_MOTORS;
    motors_overcurrent_off();
}

static void startup_beep();

void motors_init()
{
    // Repeat the motors early init routine.
    motors_early_init();

    // Charge the boost caps
    motors_all_low();
    _delay_ms(10);
    startup_beep();
    // Need to init timer after startup beep so that the timer is 
    // initialised properly, startup_beep uses it.
    motors_init_timer();
    motors_overcurrent_off();
}

static void handle_timer_overflow()
{
	// diag_puts("x\r\n");
	overflow_count += 1;
#if 0
    diag_println("mc %02x %02x", motors_commands[0].direction, motors_commands[0].duty);
#endif
}

// Previous tick data:
static uint16_t last_pwm_offset[2];

// This counts up
static uint8_t recharge_overflow_count; 
// Recharge capacitors when pwm_offset wraps every N times
#define RECHARGE_EVERY_OVERFLOWS 4

static void set_motor_outputs(uint8_t index,
	uint16_t pwm_offset)
{
	// pwm_offset is a time signal from 0... PWM_PERIOD
	bool enable=0;
	bool drivef=0, driver=0;
	bool recharge = 0;
	if (pwm_offset < last_pwm_offset[index]) {
        if (index ==0) {
            recharge_overflow_count += 1;
            if (recharge_overflow_count >= RECHARGE_EVERY_OVERFLOWS) {
                recharge_overflow_count = 0;
            }
        }
        if (recharge_overflow_count == 0) {
            recharge = 1;
        }
	}
	// Calculate pwm
	// Generate signal.
	if (motors_commands[index].brake) {
		// braking, turn on both low sides.
		enable = 1; drivef = 0; driver = 0;
	} else {
		// Scale this into the range 0..255
		uint8_t count8 = pwm_offset / 8;
		enable = (count8 <= motors_commands[index].duty);
		if (motors_commands[index].direction) {
			driver = 1; drivef = 0; // reverse
		} else {
			driver = 0; drivef = 1; // forward
		}
	}
	
	PORT_t * port_enable;
	uint8_t pin_enable;
	uint8_t pin_forward;
	uint8_t pin_reverse;
	
	if (index==0) {
		port_enable = &PORT_MOTOR_ENABLE_1;
		pin_enable = PIN_MOTOR_ENABLE_1;
		pin_forward = PIN_MOTOR1F;
		pin_reverse = PIN_MOTOR1R;
	} else {
		port_enable = &PORT_MOTOR_ENABLE_2;
		pin_enable = PIN_MOTOR_ENABLE_2;
		pin_forward = PIN_MOTOR2F;
		pin_reverse = PIN_MOTOR2R;
	}

	uint8_t bm_forward = 1 << pin_forward;
	uint8_t bm_reverse = 1 << pin_reverse;
	uint8_t bm_enable = 1 << pin_enable;

	if (recharge) {
		//Recharge then reactivate motor normally.
		PORT_MOTOR.OUTCLR = bm_forward | bm_reverse;
		port_enable->OUTSET = bm_enable;
		// Do not try to do anything else while
		// recharging the caps.
		_delay_us(RECHARGE_TIME_US);
		// Now fall through to set motor to desired speed
	}

	// Set enable pin
	if (enable) {
		port_enable->OUTSET = bm_enable;
	} else {
		port_enable->OUTCLR = bm_enable;		
	}
	// Set drive
	if (drivef) 
		PORT_MOTOR.OUTSET = bm_forward;
	else
		PORT_MOTOR.OUTCLR = bm_forward;
	if (driver)
		PORT_MOTOR.OUTSET = bm_reverse;
	else
		PORT_MOTOR.OUTCLR = bm_reverse;
	last_pwm_offset[index] = pwm_offset;
}

bool motors_loop()
{
	bool overflow=0;
	if (TCA0.SINGLE.INTFLAGS & 0x1) {
		// Overflow
		TCA0.SINGLE.INTFLAGS = 0x1; // Clears the flag
		handle_timer_overflow();
		overflow = 1;
	}
	uint16_t p1, p2; // Motor pwm cycle positions
	// range 0.. PWM_PERIOD
	p1 = TCA0.SINGLE.CNT % PWM_PERIOD;
	p2 = p1 + 1000; // Offset
	if (p2 >= PWM_PERIOD) p2 -= PWM_PERIOD;
	
    // Turn motors on, if voltage and current ok. 
    if ((overcurrent_time == 0) && vsense_ok)
    {
        set_motor_outputs(
                0, p1);
        set_motor_outputs(
                1, p2);
    }
    // NB: Do another check of overcurrent_time to avoid race:
    uint8_t oc_time = overcurrent_time;
    if (oc_time > 0) {
        if (oc_time == 1) {
            // Recharge time for the boost caps, because everything
            // was off for a while, so they are probably low on charge.
            motors_all_low();
        } else {
            // Everything off while things cool down...
            motors_overcurrent_off();
        }
    }
    
    if (! vsense_ok) {
        // Battery voltage out of range. Turn everything off.
        motors_overcurrent_off();
    }
    
    return overflow;
}

/*
 * Called from within an interrupt on overcurrent condition 
 */
void motors_overcurrent_off()
{
	PORT_MOTOR_ENABLE_1.OUTCLR = (1<< PIN_MOTOR_ENABLE_1);
	PORT_MOTOR_ENABLE_2.OUTCLR = (1<< PIN_MOTOR_ENABLE_2);
	PORT_MOTOR.OUTCLR = BITMAP_ALL_MOTORS;
}


/*
 * STARTUP TONES
 * 
 * We need to indicate to the user that both motors are working.
 * 
 * We do this by making a short, different tone on the two motors.
 * 
 * I choose A4 and D5 because it sounds vaguely nice (perfect 4th).
 * 
 */
 
#define FREQUENCY_A4 440
// Approximately 440 * (2 ** (5/12) ) - 5 semitones higher
#define FREQUENCY_D5 587

#define TCA_FREQ 1250000

static void make_tone(uint8_t index, uint16_t period)
{
    // As we alternatively pulse, use half the period
	TCA0.SINGLE.PER = period/2;	
    TCA0.SINGLE.CNT = 0;

	uint8_t pin_forward;
	uint8_t pin_reverse;
	
	if (index==0) {
		pin_forward = PIN_MOTOR1F;
		pin_reverse = PIN_MOTOR1R;
	} else {
		pin_forward = PIN_MOTOR2F;
		pin_reverse = PIN_MOTOR2R;
	}

    // Alternately pulse forwards and backwards with very short
    // pulses.
    for (uint16_t i=0; i<200; i++) {
        
        // If we detect overcurrent during the startup beep,
        // stop making any more sound.
        // e.g. if there is a short circuit at power-on
        // NB: interrupts must be enabled so overcurrent_time gets set
        if (overcurrent_time == 0) {
            if (i&1) {
                // Enable forward
                PORT_MOTOR.OUTSET = 1 << pin_forward;
            } else {
                // Enable reverse
                PORT_MOTOR.OUTSET = 1 << pin_reverse;
            }
        }
        // Wait a short time
        _delay_us(250);
        motors_all_low();
        
        // Now wait for TCA to overflow.
        uint16_t c0=0;
        while (1) {
            uint16_t c1 = TCA0.SINGLE.CNT;
            if (c1 < c0) {
                break;
            }
            c0 = c1;
        }
    }
}

static void startup_beep()
{
    if (! config_current.startup_sound_on) {
        // No startup sound.
        return;
    }
    // If voltage is not yet high enough, do not play startup sound.
    // We might still be able to work if the voltage recovers.
    if (! vsense_ok) {
        return;
    }
    // Delay for startup sound (* 100ms)
    for (uint8_t i=0; i< config_current.startup_sound_delay; ++i) {
        _delay_ms(100);
    }
    // We use the timer here.
    // TCA will now run with a period of 0.8 us or 1.25 mhz
	TCA0.SINGLE.CTRLA = (0x04 << 1) |
		1 ; // Enable bit
    motors_all_low();
    make_tone(0, (TCA_FREQ / FREQUENCY_A4) );
    make_tone(1, (TCA_FREQ / FREQUENCY_D5) );
    motors_all_low();
}
