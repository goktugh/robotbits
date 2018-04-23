include <inc/defs.scad>;
include <inc/utils.scad>;

$fs = 0.8;

use <mount_plate.scad>;

height_everywhere = 0.7;
height_edges = 1.5;

module main_plate_3d()
{
    // main();
    linear_extrude(height=height_edges, convexity=4)
    {
        union() {
            difference() 
            {
                mount_plate_main();
                offset(r=-1) mount_plate_main();  
            }
            filled_region();
        }
    }
    color("grey")
        linear_extrude(height=height_everywhere, convexity=4)
        {
            mount_plate_main();
        }
    
}

main_plate_3d();
