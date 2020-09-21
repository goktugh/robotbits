// This document uses MILLIMETRES as a unit.
// See https://hobbyking.com/en_us/3600kv.html

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.6; // millimetres
$fa = 4;

wheel_diameter = 17.0;
motor_diameter = 15.0;
centre_diameter = 10.0;
motor_can_height = 6.0; // Height of the main can
overall_height = 9.0; // Includes the top part.
top_thickness = 2.0;

// This is how much we add to the inner radius
// to compensate for 3d printer splodge, we need to change it for a
// diferent 3d printer, nozzle etc.
inner_splodge = 0.2;

module main()
{
    rotate_extrude() {
        difference() {
            // Main outer
            square([wheel_diameter/2, overall_height]);
            // Shaft cutout
            square([centre_diameter/2, overall_height + 5.0]);
            // motor can cutout
            //translate([0, (overall_height - motor_can_height)])
            //    square([motor_diameter/2, motor_can_height]);
            // inner
            translate([0, top_thickness])
                square([(motor_diameter/2) + inner_splodge, overall_height + 5.0]);
        }
    }
    linear_extrude(height=(overall_height - motor_can_height)) {
        intersection() {
            circle(r=(motor_diameter/2 + 0.5));
            // These parts stick in and match up with the can
            for (n=[0:3]) {
                rotate([0,0,(n*120)]) {
                    translate([motor_diameter/2 + 0.0, 0])
                        circle(r=5.0);
                }
            }
        }
    }
}

main();

