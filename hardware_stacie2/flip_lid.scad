/*
 * Flipper lid which actually does the flipping.
 *
 * This attaches to the flip coupler with up to 2 bolts.
 *
 * We can laser cut from plastic e.g. PS or etch and cut from Alu.
 */

include <inc/utils.scad>;

$fs = 0.6; // millimetres

coupler_w = 11;
hole_dist_x = 5.5;
hole_dist_x_half = hole_dist_x / 2;
hole_dist_y = 9; 
hole_rad = 1.25;

hole_locations = [
    [hole_dist_x_half, 0], 
    [hole_dist_x_half, 9], 
    [hole_dist_x_half, 18], 
];


module holes()
{
    mirror_x() {
        for (location = hole_locations) {
            translate(location) circle(hole_rad);
        }
    }
}

module front_clearance_holes()
{
    // Holes at the front to accomodate the front bolts, these
    // need clearance for bolt heads
    mirror_x() {
        translate([5,37])
            circle(3.5);
    }
}

module front_flat()
{
    depth = 32;
    translate([0,40 + (depth/2)]) {
        square([80,depth], center=true);
    }
}

module flip_lid_main()
{
    difference()
    {
        union()
        {
            offset(2.5)
                hull() {
                    holes();
                }
            hull() {
                front_flat();
                mirror_x() translate(hole_locations[2]) circle(hole_rad);
            }
        }
        holes();
        front_clearance_holes();
    }
    % square([11,11], center=true);
}

flip_lid_main();
