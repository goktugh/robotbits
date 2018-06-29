$fs = 0.8; // millimetres

include <inc/defs.scad>;
include <inc/utils.scad>;
use <inc/bevel_lib.scad>;

// This is a 2d object, which is the template for the armour
// from sheet materials, probably thin polypropylene

rear_holes_x_list = [body_w / 6]; // see flat1.scad
side_holes_x_list = [body_d_half -14, body_d_half + 6, body_d - 5];

thickness_margin = 0.5;

armour_height = 23; 
side_armour_height = armour_height - 1;
wedge_len = 45;
wedge_height = 2.0;
bolt_hole_r = 1.125;

module hole_tab() {
    difference() {
        intersection()
        {   
            translate([0, -9 + 10]) 
                rounded_square_centered([8,20], 3.0);
            // Ensure it does not stick up too far
            translate([-10, -11]) {
                square([20,12]);
            }
        }
        translate([0,-5 - thickness_margin])
            circle(bolt_hole_r);
    }
}

union() {
    // Rear armour:
    translate([0, armour_height / 2])
        square([body_w + thickness_margin *2, armour_height], center=true);
    mirror_x() {
        for(x = rear_holes_x_list) {
            translate([x,0]) hole_tab();
        }
    }
    // Side armour...
    mirror_x() {
        translate([body_w_half + thickness_margin,0]) {
            polygon(points=[
                [0,0],
                [0,side_armour_height],
                [body_d - wedge_len,side_armour_height],
                [body_d,wedge_height],
                [body_d,0]
                ]);
            for(x = side_holes_x_list) {
                translate([x,0]) hole_tab();
            }
        }
    }
}
