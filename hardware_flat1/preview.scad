use <flat1.scad>;
use <motor_mount_correct.scad>;
use <wheel_with_insert.scad>;
use <flipper.scad>;
include <defs.scad>;

linear_extrude(height=1.0) {
    main();
}

module motor_and_wheel() 
{
    translate([0,0,1])
        color("pink") motor_mount_main();
    // Offset 21 to wheel centre
    // Offset 5 to axle centre.
    translate([25.5,0,1 + 5])
        rotate([0,-90,0])
            wheel_with_insert();
}

module non_printed_motor()
{
    %cube([22,9,11], center=true);
}

mirror_x() {
    translate([wheel_x_front - 21,wheel_y_front,0])
        motor_and_wheel();
    translate([wheel_x_rear - 21,wheel_y_rear,0])
        motor_and_wheel();
    // Add another set of motor mounts for weapon
    // (flipper)
    translate([wheel_x_rear - 21 + 6,wheel_y_rear,12 + 1])
    {
        color("lightgreen") motor_mount_main();
        translate([-5.5,0,4.5]) non_printed_motor();
    }
}

translate([0, wheel_y_rear, 12 + 1 + 4.5])
{
    rotate([$t * 90,0, 0])
        flipper_main();
}
