use <inc/bevel_lib.scad>;
use <inc/utils.scad>;
use <motor_mount_correct.scad>;

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.8; // millimetres

mount_depth = 3.0;
cutout_depth = 13-mount_depth;

motor_length = 13;
motor_height_half = 5;

hole_offset_y = 7;
hole_offset_x = 2.5;

module motor_holder()
{
    rotate([0,-90,0])
        translate([motor_height_half - 0.2, 0,0 ]) {
            linear_extrude(height=13, convexity=3, center=true) {
                difference() {
                    offset(1.0) outline();
                    outline();
                    translate([-2 - (motor_height_half) + 0.2,0]) square([4,20], center=true);
                }
            }
        }
    // Hex pillar to support hex nut
    pillar_height = 7.0;
    mirror_y() {
        translate([hole_offset_x, hole_offset_y]) {
            cylinder(h=pillar_height, r=4.0, $fn=6);
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

module holes1()
{
    big_height = 20; // More than everything
    hex_cutout_height = 5.5;
    mirror_y() {
        translate([hole_offset_x, hole_offset_y, -0.1])
            cylinder(r=1.25, h=big_height);
        // Hex cutout (fn=6)
        translate([hole_offset_x, hole_offset_y, hex_cutout_height])
            cylinder(r=2.5, h=big_height, $fn=6);
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
            holes1();
        }
    }
}

main();
