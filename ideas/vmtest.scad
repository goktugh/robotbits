// This document uses MILLIMETRES as a unit.

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.6; // millimetres
$fa = 4;

module main()
{
    //  A cylinder; in  vase mode single wall
    linear_extrude(height=10.0) {
        circle(r=6.0);
    }
    // A piece with double wall in vase mode
    translate([0,0,10])
        linear_extrude(height=10.0) {
            difference() {
                circle(r=6.0);
                circle(r=5.0);
                translate([0,-0.5])
                    square([20,1]);

            }
        }
}

main();

