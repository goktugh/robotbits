$fs = 0.8; // millimetres

include <inc/defs.scad>;
include <inc/utils.scad>;
use <inc/bevel_lib.scad>;

use <flat1.scad>;

height_around_holes = 2.0;
height_everywhere = 0.5;

motor_w = 13;
motor_d = 22 + 0.2;
motor_height = 4.5 + height_everywhere;

module triangle_pattern(spacing, width, num)
{
    for(ang=[0,120,240]) {
        rotate([0,0,ang]) {
            for(n=[-num:num]) {
                translate([n*spacing,0]) {
                    square([width, (num*spacing)], center=true);
                }
            }
        }
    }
}

module main_flat2()
{
    // main();
    linear_extrude(height=2.0, convexity=4)
    {
        difference() 
        {
            main();
            offset(r=-1.5) main();  
        }
    }
    // Everywhere.
    
    linear_extrude(height=height_everywhere, convexity=4)
    {
        intersection() {
            main();
            triangle_pattern(14, 1.5, 12);
        }
    }
   
    // Motor mounts
    x_coord = wheel_x_front - 21;
    y_coords = [wheel_y_rear, wheel_y_front];
    
    difference() {
        linear_extrude(height=motor_height, convexity=3) {
            intersection() {
                mirror_x() {
                    for (y=y_coords ) {
                        translate([ x_coord, y]) {
                            rounded_square_centered([motor_w, motor_d], 2.0);
                        }
                    }
                }
                main();
            }
        }
        // Cutouts for wires.
        union() { // Negative
            mirror_x() {
                for (y=y_coords ) {
                    translate([ x_coord, y, 10 + motor_height - 2.0]) {
                        cube([30, 3.0, 20], center=true); // Cutout for wire
                    }
                }
            }
        }
    }
    
    
}

main_flat2();

// Example:
// ! triangle_pattern(10, 1, 10);
