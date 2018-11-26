/*
 * Based on blinky example from nrf sdk.
 */
 
#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"

const int GPIO_LED = 20; // Gpio numer of our LED.

int main(void)
{
    /* Configure board. */
    
    // NOTE: We migth need to initialise the gpio output voltage?
    // Set our gpio as an output
    nrf_gpio_cfg_output(GPIO_LED);
    // Flash the led.
    while (true)
    {
        // Use an uneven duty cycle to see the high/low
        nrf_gpio_pin_write(GPIO_LED, 1);
        nrf_delay_ms(250);
        nrf_gpio_pin_write(GPIO_LED, 0);
        nrf_delay_ms(750);
    }
}
