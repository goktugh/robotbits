// This document uses MILLIMETRES as a unit.

use <inc/bevel_lib.scad>;
use <motor_mount_correct.scad>;

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.8; // millimetres

mount_depth = 3.0;
cutout_depth = 13-mount_depth;

motor_length = 13;
motor_height_half = 5;

module motor_holder()
{
    rotate([0,-90,0])
        translate([motor_height_half - 0.2, 0,0 ]) {
            linear_extrude(height=13, convexity=3, center=true) {
                difference() {
                    offset(0.75) outline();
                    outline();
                    translate([-2 - (motor_height_half) + 0.2,0]) square([4,20], center=true);
                }
            }
        }
}

module base_part()
{
       intersection() {
            mount_outside(0.0); // base_height
            cube([30,30,4], center=true);
        }

}

module main()
{
    difference()
    {
        union() {
            base_part();
            motor_holder();
        }
        union()
        {
            motor_cutout(0.0);
            mount_holes(cutout_depth, 0.0);
        }
    }
}

main();
