// This document uses MILLIMETRES as a unit.

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.8; // millimetres

// Use the correct orientation, we can rotate it in the slicer.
// This is designed to be printed sideways (y axis=up), so that the filament will be
// in the best direction

motor_width = 12;
motor_height = 10;
motor_length = 13;
motor_length_half = motor_length / 2;
motor_width_half = motor_width / 2;
motor_height_half = motor_height / 2;
side_overlap = 4.0;

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


module mount_holes()
{
    // These holes should be along the y axis.
    translate([0,0, -1]) {
        for (x = [-0.5 * hole_distance_x, 0.5 * hole_distance_x] ) {
            for (y = [-0.5 * hole_distance_y, 0.5 * hole_distance_y]) {
                translate([x,y,0])
                    cylinder(r=1.25, h=motor_height + 3.0); 
            }
        }     
    }
}

module mount_outside()
{
    r1 = 4.0; // radius of corner
    border = 5.0;
    
    /* This is what we really want ... */
    mx = motor_length_half  - r1;
    my = motor_width_half + border - r1;
    linear_extrude(height=motor_height + 1.0) {
        hull() {
            translate([mx, my]) circle(r1);
            translate([-mx, my]) circle(r1);
            translate([mx, -my]) circle(r1);
            translate([-mx, -my]) circle(r1);
        }
    }
}


module motor_cutout()
{
    // Cut out for motor
    rotate([0,-90,0])
    translate([motor_height_half - 0.2, 0,0 ]) {
        linear_extrude(height=motor_length + 12.0, center=true) {
            outline();
        }
    }
}

module motor_mount_main() {
    difference()
    {
        mount_outside();
        union()
        {
            motor_cutout();
            mount_holes();
        }
    }
}

motor_mount_main();
// motor_cutout();
