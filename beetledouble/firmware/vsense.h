#include <stdbool.h>

extern bool vsense_ok; // false = out of range

void vsense_init();
void vsense_timer_overflow();

