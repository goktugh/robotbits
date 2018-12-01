/*
 * and idle gear used for the drive train.
 * two for each side.
 */
 
$fs = 0.6; // millimetres
$fa = 4;
 
use <MCAD/involute_gears.scad>;

module gear_outline()
{
    gear(number_of_teeth=9,
        circular_pitch = 200, flat=true,
        bore_diameter=0);
}

module main()
{
    difference() {
        gear_outline();
        circle(r=2.5);
    }
    difference() {
        square([9,1.5], center=true);
        circle(r=0.25);
    }
}

main();

// demo_3d_gears();
