
include <inc/common.inc>;
use <inc/utils.scad>;
use <inc/bevel_lib.scad>;

$fs = 0.6; // millimetres

module lid_main()
{
    difference() {
        w = (shell_w-2) - 3.4;
        d = (shell_d-2) - front_slope + 1;
        // Main lid
        union()
        {
            translate([-w / 2, -shell_d_half + 2])
                square([w,d]);
            // Rear tab part
            translate([0, -shell_d_half + 2])
                square([27,3.5], center=true);
        }
        // Holes
        mirror_x() {
            for (location = screw_locations) {
                translate(location)
                    circle(r=1.0);
            }
        }
        // Cutout for axe
        translate([0,10])
            rounded_square_centered([5,shell_d], 2);
    }
}

lid_main();
