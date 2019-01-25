$fs = 0.8; // millimetres

include <inc/defs.scad>;
include <inc/utils.scad>;
use <inc/bevel_lib.scad>;

// This is a 3d object, which will be printed in vase mode in
// Polyflex which is essentially indestructible :)

rear_holes_x_list = [body_w / 6, 7.5]; // see flat1.scad
side_holes_x_list = [body_d_half -14, body_d_half + 6, body_d - 5];

armour_height = 23; 
side_armour_height = armour_height - 1;
wedge_len = 45;
wedge_height = 2.0;
bolt_hole_r = 1.125;

shell_thickness = 1.0;
tab_thickness = 1.0;

body_w_fudge = 0.4;

module hole_tab() {
    difference() {
        intersection()
        {   
            translate([0, -7 + 10 + body_w_fudge]) 
                rounded_square_centered([6,20], 3.0);
            // Ensure it does not stick up too far
            translate([-10, -11]) {
                square([20,11.5]);
            }
        }
        translate([0,-4])
            circle(bolt_hole_r);
    }
}

module hole_tab_3d() {
    linear_extrude(tab_thickness) scale([1,-1,1]) hole_tab();
}

module armour_weight_cutouts() {
    cutout_bot = 4.0;
    cutout_top = armour_height - 2.0;
    cutout_h = cutout_top - cutout_bot;
    cutout_mid = (cutout_bot + cutout_top) / 2;
    cutout_w = (body_w * 0.3);
    cutout_thick = 0.2;
    mirror_x() {
        // Cutouts of rear armour
        translate([(cutout_w /2) + 4.0,(shell_thickness - cutout_thick*2),cutout_mid])
            cube([cutout_w, shell_thickness, cutout_h], center=true);
        // side
        translate([body_w_half + body_w_fudge + (cutout_thick/2), 45.0, cutout_mid])
            cube([cutout_thick*2, 20.0, cutout_h], center=true);
            
    }
}

module adhesion() 
{
    // Pads to make the 3d print stick to the plate.
    // To be trimmed.
    r = 6.0;
    mirror_x() {
        translate([body_w_half, 0]) {
            cylinder(r=r, h=0.35);
        }
        translate([body_w_half, body_d]) {
            cylinder(r=r, h=0.35);
        }
    }
}

module armour_3d_main()
{
    corner_rad = 6.0;
    difference() {
        union() {
            adhesion();
            
            // Rear armour:
            translate([0,0,armour_height / 2])
                cube([body_w + (body_w_fudge * 2) - (corner_rad * 2) + shell_thickness*2, shell_thickness,armour_height], center=true);
            mirror_x() {
                for(x = rear_holes_x_list) {
                    translate([x,shell_thickness]) hole_tab_3d();
                }
            }
            // Rear corners
            mirror_x() {
                translate([body_w_half - corner_rad + body_w_fudge + shell_thickness,
                    corner_rad - (shell_thickness/2),0]) {
                    linear_extrude(armour_height) {
                        difference() {
                            // Circle in the +x and -y quad
                            intersection() {
                                circle(corner_rad);
                                translate([0,-20]) square([20,20]);
                            }
                            circle(corner_rad - shell_thickness);
                        }
                    }
                }
            }
            
            // side armour
            x1 = corner_rad - (shell_thickness / 2);
            mirror_x() {
                translate([body_w / 2 + body_w_fudge,0,0])
                rotate([0,90,0]) {
                    rotate([0,0,90]) {
                        linear_extrude(shell_thickness) {
                            polygon(points=[
                                [x1,0],
                                [x1,side_armour_height],
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
                /* translate([body_w/2 + body_w_fudge, 0])
                    linear_extrude(armour_height)
                        intersection() {
                            diamond(2.5);
                            translate([-2,-0.5]) square([3,3]);
                        }
                */
            }
        }
        armour_weight_cutouts();
    }// end diff
}

armour_3d_main();
