// This document uses MILLIMETRES as a unit.

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.6; // millimetres
$fa = 4;
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
                            square([main_radius, main_height]);
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
                        flat_width = 0.7; // flat side width
                        translate([- flat_width,0,0]) {
                            side = (axle_radius + axle_radius_margin) * 2;
                            square([side, side], center=true);
                        }
                    }
                } // linear
            } // trans
            // Now we need to cut outs for spokes. These are special;
            // Each layer has three spokes, but only one of them reaches
            // the outer part.
            // This is so that on each layer, we have a continuous loop
            // with no jumps 
            spoke_cut_width = 0.7;
            for (i = [0,1,2,3,4,5]) {
                // One spoke goes *all the way*
                // One spoke nearly reaches the outside
                // One spoke nearly reaches the inside.
                rotate([0,0,i*120])  {
                    // Full spoke
                    spoke_full_len = main_radius + 10;
                    translate([0,-0.5 * spoke_cut_width,i * main_height / 6])
                        cube([spoke_full_len, spoke_cut_width, (main_height / 6) ]);
                    // Partial spoke which does not reach the outer circle
                    spoke_len_partial = main_radius - tyre_radius - 0.25;
                    rotate([0,0,120]) 
                        translate([0,-0.5 * spoke_cut_width,i * main_height / 6])
                            cube([spoke_len_partial, spoke_cut_width, (main_height / 6) ]);
                    // Partial spoke which does not reach the inner circle.
                    spoke_inner_offset = axle_radius + axle_radius_margin + 1.0;
                    rotate([0,0,240]) 
                        translate([spoke_inner_offset,-0.5 * spoke_cut_width,i * main_height / 6])
                            cube([spoke_full_len, spoke_cut_width, (main_height / 6) ]);
                    
                    
                }                
            }
        } // union
    } // difference
}

wheel_vase();

