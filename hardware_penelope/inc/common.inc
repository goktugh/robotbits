shell_w = 80;
shell_d = 84;
shell_h = 16;

drive_motor_y = -34;
flip_motor_y = 24;

front_slope = 12;
rear_slope = 0; // Set this to slope the rear backwards

shell_d_half = shell_d / 2;
shell_w_half = shell_w / 2;

screw_locations = [
    [shell_w_half - 5, 10], // front side
    [shell_w_half - 5, -4], // centre side

    [5, -shell_d_half + 2.5] // rear pair
];

