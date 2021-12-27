#include <stdbool.h>
#include <stdint.h>

extern bool vsense_ok; // false = out of range
extern uint16_t vsense_last_voltage;

void vsense_init();
void vsense_timer_overflow();

