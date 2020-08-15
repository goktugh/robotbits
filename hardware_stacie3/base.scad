
include <inc/common.inc>;
use <inc/utils.scad>;
use <inc/bevel_lib.scad>;

$fs = 0.6; // millimetres

module base_main()
{
    w = shell_w;
    // Need enough depth to bend up for front wedge.
    wedge_size = shell_h * sqrt(2);
    d = shell_d + wedge_size + 1.5; // for bendy
    
    difference() {
        translate([-w / 2, -shell_d_half + 1])
            square([w,d]);
        // Holes
        mirror_x() {
            for (location = screw_locations) {
                translate(location)
                    circle(r=1.0);
            }
        }
        // Cutout
        translate([0,-6])
            rounded_square_centered([shell_w - 15.0, shell_d - 22.0], 3.0);
        mirror_x() {
            translate([shell_w_half/2 - 2,34])
                rounded_square_centered([shell_w_half - 6, 14], 3.0);
        }
    }
}

base_main();
