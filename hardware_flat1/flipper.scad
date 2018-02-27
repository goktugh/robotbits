include <defs.scad>;
use <utils.scad>;

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.8; // millimetres

/*
this will attach to both of the top motors, which have a 3mm
shaft with flat side,  the same as the wheel motors.

When in the "down" position, it needs to have a horizontal top, and
a diagonal section; it must not foul the wheels.

For this part, the origin will be in the centre of the axle, between
the two motors.

We will render this the correct orientation, but it probably isn't
printable that way.
*/

axle_diameter = 3.0;
axle_radius = axle_diameter / 2;
axle_radius_margin = 0.1; // Extra allowance for shrinkage etc

shaft_diameter = 12.0; // 
shaft_radius = shaft_diameter / 2;
shaft_length = 14.0; // between motors / gearboxes.

flipper_width = 60;
horiz_length = 40.0; // y axis
thickness = 1.0;
bar_thickness = 2.0;
diagonal_1_length = 25;
diagonal_1_drop = 15;
diagonal_2_length = 13;
diagonal_2_drop = 13;

cutout_length = 24;
cutout_drop = 15;

module flipper_outline(chop=0)
{
   polyline([
            // Horizontal part of flipper
            [-shaft_radius, shaft_radius],
            [horiz_length, shaft_radius],
            // Diagonal part
            [horiz_length + diagonal_1_length, shaft_radius - diagonal_1_drop],
            [
                horiz_length + diagonal_1_length + diagonal_2_length - chop, 
                shaft_radius - diagonal_1_drop - diagonal_2_drop + chop
            ]
        ], 
        thickness / 2); // radius

}

module flipper_main()
{
    rotate([0,90,0]) {
        rotate([0,0,90]) {
            // Shaft attachment
            linear_extrude(height=shaft_length, center=true) {
                difference() {
                    union() {
                        circle(shaft_radius);
                        // square off the top side
                        translate([0, shaft_radius / 2])
                            square([shaft_radius*2, shaft_radius], center=true);
                    }
                    circle(axle_radius + axle_radius_margin);
                }   
            }
            // Main flipper piece
            difference() {
                union()
                {
                    // Flipper main
                    linear_extrude(height=flipper_width, center=true, convexity=3) {
                        flipper_outline();
                    }

                    // For more strength: ridge 
                    minkowski() {
                        linear_extrude(height=0.01, center=true, convexity=3) {
                            flipper_outline(chop = 5);
                        };
                        translate([0,-1,0])
                            octahedron(0.75);
                    }
                }
                
                // Cutout top pieces.
                mirror_z() {
                    translate([-shaft_radius - 10, shaft_radius - cutout_drop, (-flipper_width / 2) - 10])
                        cube(
                            [cutout_length + 10, 
                            cutout_drop + 10, 
                            ((flipper_width - shaft_length) / 2) + 10]);
                }
                // Cut off end
                translate([-shaft_radius - 10, 0,0])
                    cube([20, shaft_radius * 3, flipper_width + 10], center=true);
            }
        }
    }
}

flipper_main();
