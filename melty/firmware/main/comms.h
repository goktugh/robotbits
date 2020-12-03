#include <stdbool.h>

void comms_init();

typedef struct {
    int motor_speed;
    int pending_command;
    bool got_signal;
} comms_state_t;

extern comms_state_t comms_state; 
