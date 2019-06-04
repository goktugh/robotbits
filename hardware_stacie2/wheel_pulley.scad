use <shell.scad>;
use <MCAD/involute_gears.scad>;
use <inc/bevel_lib.scad>;
include <inc/common.inc>;

$fs = 0.1; // millimetres
$fa = 4; // degrees

drive_motor_y = 23;

WHEEL_RADIUS = 12;
WHEEL_THICKNESS = 6;
PULLEY_THICKNESS = 4.5;
PULLEY_RADIUS = 9.0;

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

module wheel_with_pulley(chamfer_thickness=0.5) {
   union() {
        wheel();
        // pulley
        translate([0,0,WHEEL_THICKNESS])
            linear_extrude(PULLEY_THICKNESS, convexity=4) {
                circle(r=PULLEY_RADIUS-0.5);
                blob_count=12;
                for (n=[0:blob_count]) {
                    rotate([0,0,(n / blob_count * 360)]) {
                        square([PULLEY_RADIUS*2, 2], center=true); 
                    }
                }
            }
        edge_thickness = 2.0;
        translate([0,0,WHEEL_THICKNESS + PULLEY_THICKNESS - edge_thickness])
            cylinder(r1=PULLEY_RADIUS - 0.5, r2=PULLEY_RADIUS + 0.5, h=edge_thickness);
    
    }
}

module wheel_on_shaft() {
    difference() {
        wheel_with_pulley(chamfer_thickness=1.0);
        // hole
        difference() {
            shaft_hole_radius = 1.5 + 0.1;
            cylinder(r=shaft_hole_radius, h=20);
            // Flat side
            translate([1.0,-3,0])
                cube([5,5,20]);
        }
        // Cutout opposite the flat side
        opposite_w = 4.5;
        opposite_h = 0.6;
        translate([-(opposite_w/2),0,5])
            cube([opposite_w, opposite_h, 20], center=true);
    }
}

module wheel_on_bearings() {
    difference() {
        wheel_with_pulley();
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
