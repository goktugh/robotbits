use <shell.scad>;
use <MCAD/involute_gears.scad>;
use <inc/bevel_lib.scad>;

$fs = 0.2; // millimetres
$fa = 4; // degrees

drive_motor_y = 23;

CP=180;
GEARWHEEL_COUNT=16;
IDLE_COUNT=12;
WHEEL_RADIUS = 12;
WHEEL_THICKNESS = 6;

module wheel() {
    tyrew = 1.0;
    rim = 0.5;
    rotate_extrude(convexity=3) {
        difference() {
            square([WHEEL_RADIUS, WHEEL_THICKNESS]);
            translate([WHEEL_RADIUS - tyrew, rim])
                bevelled_square([tyrew*2, WHEEL_THICKNESS-(rim*2)], 1.0);
        }
    }
}

module main() {
    difference() {
        union() {
            wheel();

            // Gear
            translate([0,0,WHEEL_THICKNESS])
            linear_extrude(height=2.0, convexity=4) {
                gear(number_of_teeth=GEARWHEEL_COUNT,
                    circular_pitch = CP, flat=true,
                    bore_diameter=0);    
            }
            translate([0,0,2.0 + WHEEL_THICKNESS])
            cylinder(r1=4.5,r2=4,h=0.5);
        }
        // hole
        translate([0,0,-1])
            cylinder(r=1.75, h=10);
    }
}

main();
