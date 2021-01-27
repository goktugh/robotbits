#include <stdint.h>

void timer_init();
void timer_reset();
uint32_t timer_read();
void timer_wait(uint32_t ticks);

#define TICKS_PER_MS (150L)
// For ms waits.
// tick period = ~ 6.66 us = 150 ticks per ms.
#define timer_wait_ms(ms) timer_wait(ms * TICKS_PER_MS)
