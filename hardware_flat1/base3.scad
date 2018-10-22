$fs = 0.8; // millimetres

include <inc/defs.scad>;
include <inc/utils.scad>;
use <inc/bevel_lib.scad>;

use <flat1.scad>;

height_around_holes = 2.0;
height_everywhere = 0.7;

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

module reinforcement_walls()
{
    // (2d)
    // These are short walls which reinforce the flimsy base
    // adding a little strength.
    // Y axis - walls between motor mounts
    wall_thickness = 0.6;
    mirror_x() {
        linex = wheel_x_front - 21 - 4;
        polyline([  
            [linex, wheel_y_front],
            [linex, wheel_y_rear],
            // diagonal
            [-linex, wheel_y_front]
            ],
            wall_thickness);
        // Spurs to reinforce corners
        // Front spurs
        spur_x = linex + 8;
        pi_width_half = 32.5;
        polyline([
            [spur_x + 1, wheel_y_front + 10],
            [pi_width_half -2.5 , pi_y + 13]
            ], wall_thickness);
        polyline([
            [spur_x, wheel_y_front + 10],
            [body_w_half - 2, body_d_half]
            ], wall_thickness);
        // rear
        polyline([
            [spur_x, wheel_y_rear - 10],
            [body_w_half - 12, -body_d_half]
            ], wall_thickness);
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
    // Higher fence around the outside
    linear_extrude(height=3.0, convexity=4)
    {
        difference() 
        {
            main(false);
            offset(r=-1.5) main(false);  
            // Exclude bits of perimeter which are close
            // to the holes.
            other_holes(hole_r=3.0);
        }
    }
    
    // reinforcement
    linear_extrude(height=2.0, convexity=4)
    {
        intersection() {
            reinforcement_walls();
            main();
        }
    }
    // Everywhere.
    
    color("grey")
        linear_extrude(height=height_everywhere, convexity=4)
        {
            main();
        }
   
    // Motor mounts
    x_coord = wheel_x_front - 21;
    y_coords = [wheel_y_rear, wheel_y_front];
    
    difference() {
        linear_extrude(height=motor_height, convexity=3) {
            intersection() {
                union() {
                    mirror_x() {
                        for (y=y_coords ) {
                            translate([ x_coord, y]) {
                                rounded_square_centered([motor_w, motor_d], 4.0);
                            }
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
