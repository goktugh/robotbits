use <utils.scad>;

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

raspberry_pi_outline();
