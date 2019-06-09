shell_w = 80;
shell_d = 80;
shell_h = 16;

drive_motor_y = 23;
flip_motor_y = -31;

front_slope = 12;
rear_slope = 0; // Set this to slope the rear backwards

shell_d_half = shell_d / 2;
shell_w_half = shell_w / 2;

screw_locations = [
    [5, shell_d_half - 15], // front pair

    [shell_w_half - 5, 8], // front side
    [shell_w_half - 5, -6], // centre side

    [8, -shell_d_half + 2.5] // rear pair
];

// Gears set up
CP=270; // pitch
// teeth counts
GEARWHEEL_COUNT=11;
IDLE_COUNT=8;
