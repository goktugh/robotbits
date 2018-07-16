// This document uses MILLIMETRES as a unit.

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.6; // millimetres

use <inc/bevel_lib.scad>;

// Gearbox is approx 15x10 mm, so needs about 18mm inside
main_diameter = 30;

main_radius = main_diameter / 2;

axle_diameter = 3.0;
axle_radius = axle_diameter / 2;
axle_radius_margin = 0.2; // Extra allowance for shrinkage etc
main_height = 8.5;
tyre_radius = 1.5;
tyre_offset = 2.5; // tyre distance from end

// Unit-diamond 
module diamond(r=1.0)
{
    polygon(points=[ [r,0], [0,r], [-r,0], [0,-r] ]);
}

module bevel_square(size=[10,10], r=1.0)
{
    hull() {
        translate([r,r]) diamond(r);
        translate(size - [r,r]) diamond(r);
        translate([size[0] - r, r]) diamond(r);
        translate([r, size[1] - r]) diamond(r);
    }
}

module wheel_vase() 
{
    difference()
    {
        union()
        {
            // Outer part
            rotate_extrude(convexity=3) {
                difference() {
                    
                    union() {
                        // Walls
                            bevel_square([main_radius, main_height], r=1);
                        // base
                        // square([main_radius-1, base_height]);
                    };
                    // Tyre cutout
                    translate([main_radius - (tyre_radius / sqrt(2)),0])
                    {
                        bevel_square([tyre_radius * 2, main_height],
                            r=tyre_radius);
                    }
                }
            }
        }
        
        // Cutouts
        union() 
        {
            // Cutout for axle
            translate([0,0,-10]) {
                linear_extrude(height=30.0, convexity=3) {
                    intersection() {
                        circle(r=axle_radius + axle_radius_margin);
                        translate([-0.5,0,0]) {
                            side = (axle_radius + axle_radius_margin) * 2;
                            square([side, side], center=true);
                        }
                    }
                } // linear
            } // trans
            // Now we need to cut outs for spokes. These are special;
            // We only put a single spoke on each layer, and rotate around.
            spoke_cut_width = 0.5;
            for (i = [0,1,2,3,4,5]) {
                rotate([0,0,i*120]) 
                    translate([0,-0.5 * spoke_cut_width,i * main_height / 6])
                        cube([main_radius + 10, spoke_cut_width, (main_height / 6) ]);
            }
        } // union
    } // difference
}

wheel_vase();

