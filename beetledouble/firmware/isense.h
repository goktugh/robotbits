#include <stdint.h>

void isense_init();
void isense_timer_overflow();

// If this is >0 then we are overcurrent (or were earlier)
// Keep motors off.
extern volatile uint8_t overcurrent_time;
