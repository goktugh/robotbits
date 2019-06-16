use <shell.scad>;
use <inc/bevel_lib.scad>;
use <inc/utils.scad>;

$fs = 0.2; // millimetres
$fa = 4; // degrees

main_height = 11.0; // Centre part
side_height = 4.5; 
RADIUS = 4.0;
side_radius = 2.75;
bracketlen = 36;

module outside() {
    thick = 1.5;
    panel_offset = 2.0; // Extra distance for panel
    union() {
        // Centre part
        hull() {
            cylinder(r=RADIUS,h=main_height, center=true);
            translate([RADIUS + panel_offset, 2, - (main_height / 2)])
            cube([thick, 2.5, main_height]);
        }
            
        // top-bottom
        mirror_z() {
            translate([0,0,main_height/2]) cylinder(r=side_radius,h=side_height);
        }
        /* translate([0, -RADIUS/2, - (main_height /2)])
            bevelledbox_x([RADIUS + panel_offset, RADIUS*1.5, main_height], radius=2.0);
        */
        // Panel
        translate([RADIUS + panel_offset, -RADIUS/2 + 4.0, - (main_height / 2)])
            bevelledbox_x([thick, RADIUS*1.5 + bracketlen, main_height], radius=2.0);
        // Panel: centre bracing beam
        
        translate([RADIUS-1.0, RADIUS, -1.0])
            bevelledbox_y([4,bracketlen+2, 2],radius=1.0);
    }
}

module holes() {
    // holes in the x direction through the bracket.
    hole_radius = 1.25;
    module hole1() {
        rotate([0,90,0])
            cylinder(r=hole_radius, h=30, center=true);
    }
    bl_quarter = bracketlen / 4;
    y_list = [bl_quarter*2, bl_quarter*3,
        bl_quarter*4];
    
    z = main_height / 4;
    mirror_z() {
        for (y = y_list) {
            translate([0, y, z])
                hole1();
        }
    }
}

module flip_coupler() {
    difference() {
        outside();
        // hole
        translate([0,0,-20]) {
            difference() {
                shaft_hole_radius = 1.5 + 0.1;
                cylinder(r=shaft_hole_radius, h=100);
                // Flat side
                translate([-6.0,-3,0])
                    cube([5,5,100]);
            }
        }
        holes();
    }
}

flip_coupler();
