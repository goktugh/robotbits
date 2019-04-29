
include <inc/common.inc>;
use <inc/utils.scad>;

$fs = 0.6; // millimetres

module lid_main()
{
    difference() {
        w = (shell_w-2) - 3.0;
        d = (shell_d-2) - front_slope + 1;
        union() {
            translate([-w / 2, -shell_d_half + 1])
                square([w,d]);
            mirror_x() {
                for (location = screw_locations) {
                    translate(location)
                        circle(r=2.0);
                }
            }                
        }
        
        mirror_x() {
            for (location = screw_locations) {
                translate(location)
                    circle(r=1.0);
            }
        }
        cutout_w = 12 + 1;
        cutout_d = 40 + 12;
        translate([-cutout_w /2, -shell_d_half])
            square([cutout_w, cutout_d]);
    }
}

lid_main();
