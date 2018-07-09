/*
 * This L-shaped piece goes on the left hand side of the flipper and holds the 
 * axis.
 *
 * It should use the same bolt holes as the motor mount
 */
include <inc/utils.scad>;

$fs = 0.8;

holes_dist_y = 14; // Distance between holes
holes_dist_x = 6;
bolt_hole_r = 1.2; // M2 bolts 

overall_height = 10.0;
shaft_height = 5.0; // Need to check
shaft_diameter = 3.0;
shaft_hole_diameter = shaft_diameter + 0.4;

module four_circles(radius, y_offset=0)
{
    mirror_x() mirror_y() {
        translate([(holes_dist_x / 2) , (holes_dist_y / 2) + y_offset]) {
            circle(r=radius);
        }
    }

}

module bolt_holes() {
    four_circles(bolt_hole_r);
}

module body() {
    render(convexity=4)
    difference() {
        hull() {
            // Rounded base plate
            linear_extrude(height=2.0) {
                hull() {
                    four_circles(3, 1.0); // Make it wider
                }
            }
            // Right hand side - vertical
            rhs = (holes_dist_x / 2) + 3;
            translate([rhs - 1, 0, overall_height / 2])
                cube([2,holes_dist_y,overall_height], center=true);
        }
            
        // Bolt holes
        translate([0,0,-1]) {
            linear_extrude(height=20.0) {
                bolt_holes();
            }
        }
        // hex nut cutouts.
        translate([0,0,2]) {
            linear_extrude(height=20.0) {
                $fn = 6;
                four_circles(2.5);
            }
        }
        // Chop the sides.
        overall_width = (holes_dist_x + 6);
        mirror_y() {
            translate([0, (holes_dist_y / 2) + 2.5, 12.0]) {
                cube([overall_width, 6.0, 20], center=true);
            }
        }
        
        // Hole for shaft.
        translate([0,0,shaft_height]) {
            rotate([0,90,0]) {
                linear_extrude(height=40, center=true) {
                    circle(r=shaft_hole_diameter / 2);
                }
            }
        }
    }
}

module main() {
    body();
}

main();

