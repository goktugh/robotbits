#include <stdbool.h>

void controller_init();

typedef struct {
    bool got_signal; // Do we have a signal?
    int spin_throttle; // Min = 0 max = 100
    int drive_throttle; // min = -100 max = 100
    int steering; // -100 .. 100
    bool led_test; 
    
} controller_state_t;

extern controller_state_t controller_state; 
