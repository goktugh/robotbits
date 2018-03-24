use <flat1.scad>;
use <motor_mount_correct.scad>;
use <wheel_with_insert.scad>;
use <flipper.scad>;
use <raspberry_pi.scad>;
include <inc/defs.scad>;

// Jack up the motors by this much?
base_motors_height = 4.5;

flipper_y_offset = -7.0; // move flipper back over rear wheels

// Base
linear_extrude(height=1.0) {
    main();
}

module motor_and_wheel() 
{
    translate([0,0,1])
        color("pink") render() motor_mount_main(base_height=base_motors_height);
    // Offset 21 to wheel centre
    // Offset 5 to axle centre.
    translate([25.5,0,1 + 5 + base_motors_height])
        rotate([0,-90,0])
            render() wheel_with_insert();
}

module non_printed_motor()
{
    %cube([22,9,11], center=true);
}

// Motors, wheels, motor mounts
mirror_x() {
    translate([wheel_x_front - 21,wheel_y_front,0])
        motor_and_wheel();
    translate([wheel_x_rear - 21,wheel_y_rear,0])
        motor_and_wheel();
    // Add another set of motor mounts for weapon
    // (flipper)
    // Move the flipper back half way over the rear wheels...
    translate([wheel_x_rear - 21 + 6,wheel_y_rear + flipper_y_offset,12 + 1 + base_motors_height])
    {
        color("lightgreen") motor_mount_main(base_height=0);
        translate([-5.5,0,4.5]) non_printed_motor();
    }
}

//  Flipper
translate([0, wheel_y_rear + flipper_y_offset, 12 + 1 + 4.5 + base_motors_height])
{
    rotate([$t * 90,0, 0])
        flipper_main();
}

module battery_pack()
{
    color("grey") 
        translate([0,0,6])
            cube([46,17,12], center=true);
}

// Raspberry pi
translate([0,pi_y,1.5])
    rotate([0,0,180])
        raspberry_pi();

translate([0,-4, 4.5])
    battery_pack();

// Max size cube
translate([0,-10,0])
    rotate([45,0,0]) {
         * %cube([101, 101, 101], center=true);
    }
