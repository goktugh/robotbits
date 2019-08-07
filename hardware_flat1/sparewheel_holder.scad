// This document uses MILLIMETRES as a unit.

// Set $fs (smallest arc fragment) default 2.0
$fs = 2.0; // millimetres
$fa = 4;

main_diameter = 90;

// Hex nut - will be M8 probably
nut_diameter = 13.5;
nut_depth = 10; 
hole_diameter = 8.25; // M8

main_radius = main_diameter / 2;
main_height = 27;

num_fingerholes = 10;
fingerholes_r = 12;
chamfer = 4;

module main() 
{
    difference()
    {
        // Chamfer cylinder:
        union() {
            // Bottom
            cylinder(r1=main_radius - chamfer, r2=main_radius, h=chamfer);
            // Centre
            translate([0,0,chamfer])
                cylinder(r=main_radius, h=main_height - (chamfer*2));
            // Top
            translate([0,0,main_height - chamfer])
                cylinder(r1=main_radius, r2=main_radius - chamfer, h=chamfer);
        }
        for (n = [0:num_fingerholes] ) {
            rotate( [0,0, (n/num_fingerholes) * 360 ] ) {
                translate([main_radius, 0, main_height]) {
                    scale([1.5, 1, 1]) sphere(r=fingerholes_r);
                }
            }
        }
        // Cut out the bolt hole
        cylinder(r=(hole_diameter/2), h=100);
        // Cutout for nut
        translate([0,0,main_height - nut_depth])
            cylinder(r=(nut_diameter/2), h=100, $fn=6);
    }
}

main();

