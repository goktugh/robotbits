use <inc/bevel_lib.scad>;

// $fa is related to arcs
$fa = 0.8; // millimetres

// This radius should be a little less than our wheels, to allow for
// stretching.
wheel_radius = 12.0;
wheel_width = 6.0;

tyre_thickness = 2.5; // Includes knobbles

button_radius = 2.5;

/*
 * To make a tyre mould, we need two pieces, outer and inner.
 */
 
module mould_inner()
{
    //  This will be the "correct way up"
    rotate_extrude(convexity=3) {
        square([wheel_radius + 5, 1]);
        square([wheel_radius, wheel_width + 1]);
        // Piece to push to release the mould.
        translate([0,wheel_width +0.5]) {
            square([button_radius,5]);
        }
        translate([0,wheel_width +5]) {
            intersection() {
                square([5,5]);
                circle(button_radius, $fn=30);
            }
        }
    }
    // translate([0,0,wheel_width+4.5]) {
    //     sphere(2.0);
    // }
}

module mould_outer()
{
    // This will be upside-down.
    overall_height = wheel_width + 1;
    hole_radius = button_radius + 0.2;
    rotate_extrude(convexity=3) {
        // Base
        translate([hole_radius,0])
            square([wheel_radius + tyre_thickness - hole_radius, 1]);
        // Side wall
        translate([wheel_radius + tyre_thickness, 0]) {
              square([1, wheel_width+1]);
        }
    }
    knobble_size = 1.0;
    knobble_count = 36;
    // Knobbly tread
    for(ang = [0:(360 / knobble_count) :360]) {
        rotate([0,0,ang]) {
            translate([wheel_radius + tyre_thickness,0,overall_height / 2]) {
                rotate([0,0,45])
                    cube([knobble_size,knobble_size, overall_height], center=true);
            }
        }
    }
}

module main()
{
    union() {
        mould_inner();
        translate([wheel_radius * 2 + tyre_thickness + 7,0,0]) {
            mould_outer();
        }
    }
}

main();
