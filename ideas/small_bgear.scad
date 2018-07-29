// This document uses MILLIMETRES as a unit.

use <MCAD/involute_gears.scad>;

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.6; // millimetres
$fa = 4;

// Small bevel gear.

module main()
{
    // Pitch angle must 45, for shafts at a right andle.
    
    outside_circular_pitch = 200;
    gear_teeth = 7;
    outside_pitch_radius = gear_teeth * outside_circular_pitch / 360;
    pitch_apex=outside_pitch_radius; // For 90 degree axes
                
    cone_distance = sqrt (pow (pitch_apex, 2) + pow (outside_pitch_radius, 2));
    
    bevel_gear(
        bore_diameter=3.0,
        face_width=4.0, 
        gear_thickness = 6.0,
        cone_distance=cone_distance,
        // pressure_angle=45,
        outside_circular_pitch=outside_circular_pitch,
        number_of_teeth=gear_teeth,
        finish=0
        );
    translate([0,0,-7])
        %linear_extrude(height = 5.0) {
            circle(r=5.0);
    }
}

main();

