
#include <stdbool.h>
#include <stdint.h>

typedef struct {
	bool direction; // false = fwd, true = rev
	uint8_t duty;
	bool brake;
} motor_command_t;

void motors_init();

void motors_loop();
