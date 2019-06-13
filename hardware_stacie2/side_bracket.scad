include <inc/common.inc>;
include <inc/bevel_lib.scad>;

$fs = 0.2; // millimetres

bracket_height = 11.5; // Must be a little higher than the wheel thickness inc. pulley.
bracket_thickness = 8.0;
bolt_dist = 14.0; // Distance between bolt centres.
bolt_radius = 1.6; // inncludes some splodge

module side_bracket_main()
{
    overlap = 1.4;
    linear_extrude(bracket_height) {
        difference() {
            translate([-overlap,-(bracket_thickness /2)])
                bevelled_square([bolt_dist + (2*overlap), bracket_thickness], radius = 1.0); 
            circle(r=bolt_radius);
            translate([bolt_dist,0]) circle(r=bolt_radius);
        }
    }
}

side_bracket_main();
