use <shell.scad>;
use <MCAD/involute_gears.scad>;

$fs = 0.2; // millimetres
$fa = 4; // deg

drive_motor_y = 23;

CP=180;
GEARWHEEL_COUNT=16;
IDLE_COUNT=12;

module main() {
    difference() {
        union() {
            cylinder(r1=4,r2=4.5,h=0.5);
            translate([0,0,0.5])
            linear_extrude(height=1.0, convexity=4) {
                gear(number_of_teeth=IDLE_COUNT,
                    circular_pitch = CP, flat=true,
                    bore_diameter=0);    
            }
            translate([0,0,1.5])
            cylinder(r1=4.5,r2=4,h=0.5);
        }
        // hole
        translate([0,0,-1])
            cylinder(r=0.75, h=10);
    }
}

main();
