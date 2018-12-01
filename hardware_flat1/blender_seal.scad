use <inc/bevel_lib.scad>;

// $fa is related to arcs
$fa = 0.8; // millimetres

inner_radius = 24.5;
width = 2.5;

tyre_thickness = 5.0; // Includes knobbles

button_radius = 2.5;

/*
 * To make a tyre mould, we need two pieces, outer and inner.
 */
 
module mould_inner()
{
    //  This will be the "correct way up"
    rotate_extrude(convexity=3) {
        square([inner_radius + 5, 1]);
        square([inner_radius, width + 1]);
        // Piece to push to release the mould.
        translate([0,width +0.5]) {
            square([button_radius,5]);
        }
        translate([0,width +5]) {
            intersection() {
                square([5,5]);
                circle(button_radius, $fn=30);
            }
        }
    }
}

module mould_outer()
{
    // This will be upside-down.
    overall_height = width + 1;
    hole_radius = button_radius + 0.4;
    side_thickness = 2.0;
    rotate_extrude(convexity=3) {
        // Base
        translate([hole_radius,0])
            square([inner_radius + tyre_thickness - hole_radius, 1]);
        // Side wall
        translate([inner_radius + tyre_thickness, 0]) {
              square([side_thickness, width+1]);
        }
    }
}

module main()
{
    union() {
        mould_inner();
        translate([inner_radius * 2 + tyre_thickness + 8,0,0]) {
            mould_outer();
        }
    }
}

main();
