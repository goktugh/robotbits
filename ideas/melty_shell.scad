// This document uses MILLIMETRES as a unit.

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.6; // millimetres
$fa = 4;

/*
 Melty brain spinner cannot have a diameter of much more than
 100mm because of the size of the cube. It is technically theoretically
 possible to increase this a little by using the "inner diagnonal"
 if the robot is not too high.
*/

outer_diameter = 100.0;
inner_diameter = 60.0;
// Height must not be too high, so our tyres can stick out the top/
// bottom.
overall_height = 17.0; 

inner_radius = inner_diameter / 2;
outer_radius = outer_diameter / 2;
chamfer = 1.0;
wall_thick = 3.0;
base_thick = 2.0;

module walls_outline()
{
        polygon(points = [
            [inner_radius+chamfer, 0],
            [inner_radius, chamfer],            
            [inner_radius, overall_height-chamfer],
            [inner_radius+chamfer, overall_height],

            [inner_radius+wall_thick - chamfer, overall_height],
            [inner_radius+wall_thick, overall_height - chamfer],
            [inner_radius+wall_thick, base_thick],
            [outer_radius-wall_thick, base_thick],
            [outer_radius-wall_thick, overall_height - chamfer],
            [outer_radius-wall_thick + chamfer, overall_height],

            [outer_radius-chamfer, overall_height],
            [outer_radius, overall_height-chamfer],
            [outer_radius, chamfer],
            [outer_radius-chamfer, 0]
            ], convexity=3);
}

module shell_limits()
{
    rotate_extrude() {
        polygon(points = [
            [inner_radius, 0],
            [inner_radius, overall_height],
            [outer_radius, overall_height],
            [outer_radius, 0]
            ], convexity=3);
    }
}

motor_cutout_w = 30;

module motor_cutouts()
{
    // Cut down the wall for the motor mount
    translate([inner_radius-5.0, - (motor_cutout_w/2), base_thick + 0.5]) {
        cube([10, motor_cutout_w, overall_height]);
    }
}

module main()
{
    difference() {
        rotate_extrude(convexity=4) {
            walls_outline();
        }
        motor_cutouts();
    }
}

main();

