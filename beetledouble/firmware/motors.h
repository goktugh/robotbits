
#include <stdbool.h>
#include <stdint.h>

#define MOTORS_COUNT 2

typedef struct {
	bool direction; // false = fwd, true = rev
	uint8_t duty; // max=255
	bool brake;
} motor_command_t;

extern motor_command_t motors_commands[MOTORS_COUNT];

void motors_init();

// Returns true on timer overflow
bool motors_loop();
