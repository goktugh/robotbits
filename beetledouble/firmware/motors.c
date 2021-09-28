#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#define F_CPU 10000000 /* 10MHz / prescale=2 */
#include <util/delay.h>

#include "diag.h"

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
#define PIN_MOTOR_ENABLE_1 0

#define PORT_MOTOR_ENABLE_2 PORTB
#define PIN_MOTOR_ENABLE_2 1

// Gate drive pins

#define PORT_MOTOR PORTA

#define PIN_MOTOR1F 4
#define PIN_MOTOR1R 5

#define PIN_MOTOR2F 6
#define PIN_MOTOR2R 7

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

// PWM_PERIOD should be a divisor of TCA_PERIOD.
// this is the period of our duty cycle pulses.
// Could be a power of two for faster divide
#define PWM_PERIOD (4096)

static void motors_init_timer()
{
	/*
	 * We use TCA in the simplest mode possible with no output generation
	 * and no interrupts, just poll its counts.
	 */
	// TCA - divide by 16 diving a period of 1.6us
	// Count up to 62500 giving a wraparound time of
	// 100000 us or 100ms
	// Clock selector: divide by 16.
	TCA0.SINGLE.PER = TCA_PERIOD;	
	TCA0.SINGLE.CTRLA = (0x04 << 1) |
		1 ; // Enable bit
}

void motors_init()
{
	motors_init_timer();
	
	// Make enable pins outputs.
	PORT_MOTOR_ENABLE_1.DIRSET = (1<< PIN_MOTOR_ENABLE_1); 
	PORT_MOTOR_ENABLE_2.DIRSET = (1<< PIN_MOTOR_ENABLE_2);
	
	// Make the gate drive pins outputs.
	PORT_MOTOR.DIRSET = (
		(1<< PIN_MOTOR1F) | 
		(1<< PIN_MOTOR1R) | 
		(1<< PIN_MOTOR2F) | 
		(1<< PIN_MOTOR2R)
		);
	// Initially charge the bst caps:
	PORT_MOTOR.OUTCLR = BITMAP_ALL_MOTORS;
	PORT_MOTOR_ENABLE_1.OUTSET = (1<< PIN_MOTOR_ENABLE_1);
	PORT_MOTOR_ENABLE_2.OUTSET = (1<< PIN_MOTOR_ENABLE_2);
	
	_delay_ms(10);
	PORT_MOTOR_ENABLE_1.OUTCLR = (1<< PIN_MOTOR_ENABLE_1);
	PORT_MOTOR_ENABLE_2.OUTCLR = (1<< PIN_MOTOR_ENABLE_2);
	
}

static void handle_timer_overflow()
{
	// diag_puts("x\r\n");
	overflow_count += 1;
}

static void set_motor_outputs(uint8_t index,
	uint16_t pwm_offset)
{
	// pwm_offset is a time signal from 0... PWM_PERIOD
	bool enable=0;
	bool drivef=0, driver=0;
	// Recharge capacitors:
	if (pwm_offset< 80) {
		enable=1;
		drivef=0; driver=0;
	} else {
		// Generate signal.
		// TODO: Run duty cycle / direction
		enable = (overflow_count & 0x40);
		if (overflow_count & 0x100) {
			drivef = 1;
			driver = 0;
		} else {			
			drivef = 0;
			driver = 1;
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
	// Set enable pin
	if (enable) {
		port_enable->OUTSET = 1 << pin_enable;
	} else {
		port_enable->OUTCLR = 1 << pin_enable;		
	}
	// Set drive
	if (drivef) 
		PORT_MOTOR.OUTSET = 1 << pin_forward;
	else
		PORT_MOTOR.OUTCLR = 1 << pin_forward;
	if (driver)
		PORT_MOTOR.OUTSET = 1 << pin_reverse;
	else
		PORT_MOTOR.OUTCLR = 1 << pin_reverse;
}

void motors_loop()
{
	if (TCA0.SINGLE.INTFLAGS & 0x1) {
		// Overflow
		TCA0.SINGLE.INTFLAGS = 0x1; // Clears the flag
		handle_timer_overflow();
	}
	uint16_t p1, p2; // Motor pwm cycle positions
	// range 0.. PWM_PERIOD
	p1 = TCA0.SINGLE.CNT % PWM_PERIOD;
	p2 = p1 + 1000; // Offset
	if (p2 > PWM_PERIOD) p2 -= PWM_PERIOD;
	
	set_motor_outputs(
			0, p1);
	set_motor_outputs(
			1, p2);
}
