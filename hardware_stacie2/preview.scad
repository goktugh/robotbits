use <shell.scad>;
use <MCAD/involute_gears.scad>;
use <flip_coupler.scad>;
use <lid.scad>;
use <outerpanel.scad>;
use <flip_lid.scad>;
include <inc/common.inc>;

$fs = 0.6; // millimetres

drive_motor_y = 23;

module gearwheel() {
    linear_extrude(height=4) {
        difference() {
            gear(number_of_teeth=GEARWHEEL_COUNT,
                circular_pitch = CP, flat=true,
                bore_diameter=0);    
            circle(1.5);
        }
    }
    %translate([0,0,4]) {
        cylinder(h=6, r=12);
    }
}

module idlegear()
{
    color("pink")
    rotate([0,0,(360/IDLE_COUNT/2)] )
    linear_extrude(height=4) {
        difference() {
            gear(number_of_teeth=IDLE_COUNT,
                circular_pitch = CP, flat=true,
                bore_diameter=0);    
            circle(1.0);
        }
    }
}

module wheels() {
    translate([40,0,8]) {   
        translate([0,drive_motor_y,0]) {
            rotate([0,90,0]) gearwheel();
        }
        /*
        translate([0,8.5,0]) {
            rotate([0,90,0]) idlegear();
        }
        translate([0,-6,0]) {
            rotate([0,90,0]) gearwheel();
        }
        translate([0,-20.5,0]) {
            rotate([0,90,0]) idlegear();
        }
        */
        translate([0,-35,0]) {
            rotate([0,90,0]) gearwheel();
        }
    }
}

// Shell
color("lightblue") shell_main();
wheels();


// Flip coupler
color("lightgreen") {
    translate([0,-31,9])
        rotate([0,-90,0])
            flip_coupler();
}

// lid

/*
translate([0,0,20])
    linear_extrude(height=1)
        lid_main();
*/
// Flip lid

translate([0,0,25])
    linear_extrude(height=1)
        translate([0,-13])
            flip_lid_main();
        
// side panels
translate([-45,0,shell_h/2]) {
    rotate([0,90,0]) 
    {
        linear_extrude(height=2) {
            outerpanel();
        }
    }
        
}
