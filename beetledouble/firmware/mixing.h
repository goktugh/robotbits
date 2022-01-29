
#include <stdint.h>

/*
 * Called to set the channel speed, a signed number
 * in the range  
 */
void mixing_set_speed(uint8_t channel, int16_t speed);
/*
 * Called to set the channel idle if no pulses are received.
 */
void mixing_set_idle(uint8_t channel);

void mixing_init();
