/*
 * and idle gear used for the drive train.
 * two for each side.
 */
 
$fs = 0.6; // millimetres
$fa = 4;
 
use <MCAD/involute_gears.scad>;

module wheel_gear_outline()
{
    gear(number_of_teeth=18,
        circular_pitch = 200, flat=true,
        bore_diameter=0);
}

module wheel_gear_main()
{
    wheel_gear_outline();
}

wheel_gear_main();

