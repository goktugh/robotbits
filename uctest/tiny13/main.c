#include <avr/io.h>

// attiny13 default clock is nominally 9.6mhz
#define F_CPU 9600000
#include <util/delay.h>

int main()
{
    uint8_t blink_bit = 1 << 0;
    DDRB |= blink_bit; // Enable output on PB0 

    while(1) {
        PORTB |= blink_bit;
       _delay_ms(500); 
        PORTB &= ~blink_bit;
       _delay_ms(500); 

    }
}
