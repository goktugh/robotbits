use <inc/bevel_lib.scad>;

// $fs is minimium arc fragment size
$fs = 0.1; // millimetres
$fa = 1.0; // degrees

// This radius should be a little less than our wheels, to allow for
// stretching.
wheel_radius = 13.5; // our wheels are really 30mm dia
wheel_width = 6.0;

tyre_thickness = 1.8;
 
module tyre1()
{
    knobble_size = 1.0;
    knobble_count = 36;
    // Knobbly tread
    union() {
        linear_extrude(height=wheel_width) {
            difference() {
                union() {
                    for(ang = [0:(360 / knobble_count) :360]) {
                        rotate([0,0,ang]) {
                            translate([wheel_radius + tyre_thickness - 0.1,0])
                                rotate([0,0,45])
                                    square([knobble_size,knobble_size], center=true);
                        }
                    }
                    circle(wheel_radius + tyre_thickness);
                }                
                circle(wheel_radius);
            }
        }
    }
}

module main()
{
    tyre1();
}

main();
