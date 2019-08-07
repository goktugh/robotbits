include <inc/common.inc>;

use <shell.scad>;

projection(cut=true) 
    rotate([0,90,0]) 
        translate([shell_w_half - 0.5,0,0]) shell_main(true);
