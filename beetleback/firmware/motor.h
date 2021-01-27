#include <stdint.h>

void motor_init();

void motor_set_speed_signed(int16_t speed);

void motor_set_brake();
void motor_off();
