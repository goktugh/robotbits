use <inc/utils.scad>;

$fs = 0.8;

/* Raspberry Pi zero */

pi_w = 65;
pi_w_half = pi_w / 2;
pi_h = 30;
pi_h_half = pi_h / 2;
pi_corner_radius = 3;

pi_hole_radius = 1.375;
pi_hole_offset = 3;

//  Centered.
module raspberry_pi_outline() {
    difference() {
        rounded_rect(pi_w, pi_h, pi_corner_radius);
        // Mounting holes
        mirror_x() {
            translate([pi_w_half - pi_hole_offset, pi_h_half - pi_hole_offset])
                circle(pi_hole_radius);
            translate([pi_w_half - pi_hole_offset, -pi_h_half + pi_hole_offset])
                circle(pi_hole_radius);
        }
    }
}

module raspberry_pi() {
    color("green")
    linear_extrude(height=1.0) {
            raspberry_pi_outline();
    }
    // Gubbins
    color("gray") {
        // SD card
        translate([-pi_w_half - 1.5, -6, 1]) cube([12,10,1]);
        // hdmi connector
        translate([-pi_w_half + 12.4, -pi_h_half -0.5, 1]) cube([10,6,1]);
        // 2x micro-usb
        translate([-pi_w_half + 41.4, -pi_h_half -0.5, 1]) cube([6,4,1]);
        translate([-pi_w_half + 54.0, -pi_h_half -0.5, 1]) cube([6,4,1]);
    }
    
}

raspberry_pi();
