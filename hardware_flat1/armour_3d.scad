$fs = 0.8; // millimetres

include <inc/defs.scad>;
include <inc/utils.scad>;
use <inc/bevel_lib.scad>;

// This is a 3d object, which will be printed in vase mode in
// Polyflex which is essentially indestructible :)

rear_holes_x_list = [body_w / 6]; // see flat1.scad
side_holes_x_list = [body_d_half -14, body_d_half + 6, body_d - 5];

armour_height = 23; 
side_armour_height = armour_height - 1;
wedge_len = 45;
wedge_height = 2.0;
bolt_hole_r = 1.125;

shell_thickness = 1.0;
tab_thickness = 1.0;

module hole_tab() {
    difference() {
        intersection()
        {   
            translate([0, -7 + 10]) 
                rounded_square_centered([8,20], 3.0);
            // Ensure it does not stick up too far
            translate([-10, -11]) {
                square([20,11]);
            }
        }
        translate([0,-4])
            circle(bolt_hole_r);
    }
}

module hole_tab_3d() {
    linear_extrude(tab_thickness) scale([1,-1,1]) hole_tab();
}


module armour_3d_main()
{
    union() {
        // Rear armour:
        translate([0,0,armour_height / 2])
            cube([body_w, shell_thickness,armour_height], center=true);
        mirror_x() {
            for(x = rear_holes_x_list) {
                translate([x,0]) hole_tab_3d();
            }
        }
        // side armour
        mirror_x() {
            translate([body_w / 2,0,0])
            rotate([0,90,0]) {
                rotate([0,0,90]) {
                    linear_extrude(shell_thickness) {
                        polygon(points=[
                            [0,0],
                            [0,side_armour_height],
                            [body_d - wedge_len,side_armour_height],
                            [body_d,wedge_height],
                            [body_d,0]
                            ]);            
                    }
                }
            }
            rotate([0,0,90])
                for(y = side_holes_x_list) {
                    translate([y,-(body_w /2)]) hole_tab_3d();
                }
            // Corner supports
            translate([body_w/2, 0])
                linear_extrude(armour_height)
                    intersection() {
                        diamond(2.0);
                        translate([-2,-0.5]) square([3,3]);
                    }
        }
    }
}

armour_3d_main();
