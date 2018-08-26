use <base3.scad>;
use <armour_3d.scad>;
include <inc/defs.scad>;

main_flat2();

translate([0,-body_d / 2,-1.5])
{
    color("pink")
        armour_3d_main();
}
