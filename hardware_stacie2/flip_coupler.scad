use <shell.scad>;
use <inc/bevel_lib.scad>;
use <inc/utils.scad>;

$fs = 0.2; // millimetres
$fa = 4; // degrees

main_height = 11.0; // Centre part
total_height = 16.0; 
RADIUS = 4.0;
side_radius = 2.75;
bracketlen = 36;

module flip_coupler_centre()
{
    // Across flats 8.0
    af = 8.0;
    r1 = (af / 2 / cos(30));
    thick = 1.5;
    panel_offset = 2.0; // Extra distance for panel
    // Centre part
    difference() {
        // Outer
         hull() {
            cylinder(r=r1 + 0.5, h=total_height, $fn=6, center=true);
            translate([RADIUS + panel_offset, 2, - (main_height / 2)])
            cube([thick, 2.5, main_height]);            
        }
         // Cutout for the alu shaft adapter
         cylinder(r=r1, h=30.0, $fn=6, center=true);
       }
}

module outside() {
    thick = 1.5;
    panel_offset = 2.0; // Extra distance for panel
    union() {
        flip_coupler_centre();

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
        holes();
    }
}

module hexshaft() {
    // Across flats 8.0
    af = 8.0;
    r = (af / 2 / cos(30));
    rotate([0,0,30]) cylinder(r=r, h=20.0, $fn=6, center=true);
}

flip_coupler();
// %hexshaft();
