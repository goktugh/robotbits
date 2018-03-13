// This document uses MILLIMETRES as a unit.

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.6; // millimetres

// Gearbox is approx 15x10 mm, so needs about 18mm inside
main_diameter = 30;

main_radius = main_diameter / 2;

axle_diameter = 3.0;
axle_radius = axle_diameter / 2;
axle_radius_margin = 0.2; // Extra allowance for shrinkage etc
axle_height = 8.5; // It is nominally 9mm
main_height = 8.5;
base_height = main_height / 2;
main_thickness = 1.8;
tyre_radius = 1.5;
tyre_offset = 2.5; // tyre distance from end

// Cutout for grub screw nut for axle flat:
axle_nut_h = 1.25;
axle_nut_w = 5.0;
grub_screw_radius = 1.25;
driver_radius = 2.0;

/*
 * This wheel wraps around the gearbox slightly. It mounts on the axle
 * in the normal way.
 *
 * Therefore we need the wheel outer to be longer than the axle mount,
 * the direction of the print is:
 * +Z points INTO the motor, which might not be what you expect. 
 */
 

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


module wheel_with_insert() 
{
    difference()
    {
        union()
        {
            // Axle mount
            cylinder(h=axle_height, r=axle_radius + 3.5);
            
            // Outer part
            rotate_extrude(convexity=3) {
                difference() {
                    
                    union() {
                        // Walls
                        translate([main_radius - main_thickness,0])
                            bevel_square([main_thickness, main_height], r=1);
                        // base
                        square([main_radius-1, base_height]);
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
                    union() {
                        circle(r=axle_radius + axle_radius_margin);
                        translate([1.0 + (axle_nut_h / 2),0])
                            square([axle_nut_h, axle_nut_w], center=true);
                    }
                } // linear
            } // trans
        } // union
        // Cutout for the screw / screwdriver.
        translate([0,0, main_height/2])
        rotate([0,90,0]) {
            cylinder(h=100, r = grub_screw_radius);
            // Cutout for screwdriver:
            // Not needed if we use a hex grub screw.
            //translate([0,0, axle_radius + 3.0])
            //    cylinder(h=100, r = driver_radius);
            // Cutout for screw head
            translate([0,0, axle_radius + 4.0])
                cylinder(h=6.0, r=2.75);
        }
    } // diff
    
    // Non printed
    %translate([0,0,axle_height + 0.5 + 11]) {
        cube([11,9,22], center=true);
    }
}

wheel_with_insert();

