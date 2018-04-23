// This document uses MILLIMETRES as a unit.

use <inc/bevel_lib.scad>;

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.8; // millimetres

// Use the correct orientation, we can rotate it in the slicer.
// This is designed to be printed sideways (y axis=up), so that the filament will be
// in the best direction

base_height_main = 0.0; // Set this with -D

motor_width_margin = 0.2;
motor_width = 12 + motor_width_margin;
motor_height = 10;
overall_height = motor_height + 1.0; // Needs to be bigger than the motor!
motor_length = 13;
motor_length_half = motor_length / 2;
motor_width_half = motor_width / 2;
motor_height_half = motor_height / 2;
side_overlap = 4.0;
cutout_r = 2.125;

hole_distance_y = 14;
hole_distance_x = 6;

module outline() {
    intersection() {
        square([motor_height, motor_width], center=true);
        rad1 = motor_width - 3;
        translate([0, - motor_width_half + rad1]) 
            circle(r=rad1);
        translate([0, motor_width_half - rad1]) 
            circle(r=rad1);
    }
}


module mount_holes(cutout_depth, base_height)
{
    // These holes should be along the z axis.
    translate([0,0, -1]) {
        for (x = [-0.5 * hole_distance_x, 0.5 * hole_distance_x] ) {
            for (y = [-0.5 * hole_distance_y, 0.5 * hole_distance_y]) {
                translate([x,y,0])
                    cylinder(r=1.25, h=overall_height + 1.0 + base_height + 0.1 - cutout_depth);
                echo("cutout_depth=", cutout_depth);
                if (cutout_depth > 0.01) {
                    $fn = 6; // Hex cutout
                    // Because it is a hex, we actually need to increase
                    // r, otherwise it's too small.
                    cutout_calculated_r = cutout_r * 1.3;
                    // Hex cutouts - rotate 30 degrees? to give a better
                    // orientation.
                    translate([x,y,overall_height + 1.0 + base_height - cutout_depth])
                        rotate([0,0,0])
                            cylinder(r=cutout_calculated_r, h = 3.0);
                }
            }
        }     
    }
    if (base_height > 2.0) {
        wiring_w = 5.0;
        wiring_h = 1.5;
        // Cutout underneath for wiring... 
        // in x direction
        translate([-20, -(wiring_w / 2), - wiring_h])
            bevelledbox_x([40, wiring_w, wiring_h*2], radius=wiring_h); 
    }
}

module mount_outside(base_height)
{
    r1 = 4.0; // radius of corner
    border = 5.0;
    
    /* This is what we really want ... */
    mx = motor_length_half  - r1;
    my = motor_width_half + border - r1;
    linear_extrude(height=overall_height + base_height) {
        hull() {
            translate([mx, my]) circle(r1);
            translate([-mx, my]) circle(r1);
            translate([mx, -my]) circle(r1);
            translate([-mx, -my]) circle(r1);
        }
    }
}


module motor_cutout(base_height)
{
    // Cut out for motor
    translate([0,0,base_height]) 
        rotate([0,-90,0])
            translate([motor_height_half - 0.2, 0,0 ]) {
                linear_extrude(height=motor_length + 12.0, center=true) {
                    outline();
                }
            }
}

module motor_mount_main(cutout_depth = 0.0, base_height=0.0) {
    difference()
    {
        mount_outside(base_height);
        union()
        {
            motor_cutout(base_height);
            mount_holes(cutout_depth, base_height);
        }
    }
}

motor_mount_main(base_height = base_height_main);
