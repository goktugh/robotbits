use <shell.scad>;
use <MCAD/involute_gears.scad>;
use <inc/bevel_lib.scad>;
include <inc/common.inc>;

$fs = 0.1; // millimetres
$fa = 4; // degrees

drive_motor_y = 23;

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

module wheel_with_gear() {
    //offset for splodge compensation
    splodge_compensation = -0.2;
   union() {
        wheel();

        // Gear
        translate([0,0,WHEEL_THICKNESS])
        linear_extrude(height=2.0, convexity=4) {
            offset(splodge_compensation) 
                gear(number_of_teeth=GEARWHEEL_COUNT,
                    circular_pitch = CP, flat=true,
                    bore_diameter=0);    
        }
        translate([0,0,2.0 + WHEEL_THICKNESS])
        cylinder(r1=5.5,r2=5,h=0.5);
    }
}

module wheel_on_shaft() {
    difference() {
        wheel_with_gear();
        // hole
        difference() {
            shaft_hole_radius = 1.5 + 0.4;
            cylinder(r=shaft_hole_radius, h=10);
            // Flat side
            translate([1.0,-3,0])
                cube([5,5,10]);
        }
    }
}

module wheel_on_bearings() {
    difference() {
        wheel_with_gear();
        // hole, 3mm
        // Skip the hole, we can drill it out.
        // cylinder(r=1.7, h=10);
        // Bearing cutouts, 8mm * 4mm
        bearing_radius = 4.0 + 0.2; // Allow for squidge factor
        translate([0,0,-0.1])
            cylinder(r=bearing_radius, h=4.1);
        translate([0,0,WHEEL_THICKNESS + 2.5 - 4.0])
            cylinder(r=bearing_radius, h=4.1);
            
    }
}

wheel_on_shaft();
// wheel_on_bearings();
