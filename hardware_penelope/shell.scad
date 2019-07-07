/*
 * Penelope the axe-bot
 */
 
use <n20.scad>;
use <inc/utils.scad>;
use <inc/bevel_lib.scad>;

include <inc/common.inc>;

$fs = 0.6; // millimetres

module mirror_xy() {
    mirror_x() {
        mirror_y() {
            children();
        }
    }
}

module side_outline() {
    // Make the quad for the main shape
    // 2d, centred on the x axis
    polygon(
        [
            [-shell_d_half, 0],
            [-shell_d_half - rear_slope, shell_h],
            [shell_d_half - front_slope, shell_h],
            [shell_d_half, 0]
        ]
        );
}

module motor_cutouts() {
    mirror_x() {
        // Drive motors
        translate([shell_w_half,drive_motor_y,shell_h/2 -0.5]) {
            n20_motor_top_ext();
        }
        // Flip motors - these point inwards.
        translate([-10,flip_motor_y,shell_h/2]) {
            n20_motor_top_ext(back_part=3.0);        
        }
    }
}

module lid_cutout() {
    translate( [0,0,(shell_h * 1.5) - 2]) {
        cube([shell_w - 4, shell_d - 4, shell_h], center=true);
    }
}

module other_cutouts() {
    // Cutouts: Battery / gubbins compartment
    // fullybevelledbox is not centred.
    mirror_x() {
        translate([2, -26, 1 + 0.4])
            fullybevelledbox([31.5,41, shell_h -3], radius=1.0);
    }
    // Lengthwise cutout
    translate([-1.25,-shell_d_half + 3, 9])
        fullybevelledbox([2.5, shell_d - 10, shell_h], radius=1.0);
    // Cutout for the weapon axle etc
    cutout_axle_depth = 9;
    translate([-11, flip_motor_y - (cutout_axle_depth/2), shell_h - 12.5])
        fullybevelledbox([22, cutout_axle_depth, shell_h], radius=1.0);
        
    // Cutout for the weapon axle bits
    translate([-6, flip_motor_y - 17, -5])
        fullybevelledbox([12, 33, 32], radius=2.0);
    // Cut out the rear
    translate([0, -shell_d_half, shell_h])
        cube([28,5,4], center=true);
}

module screw_holes() {
    // We could use M2 bolts, self tappers or something else.
    r=1.2;
    mirror_x() {
        for (location = screw_locations) {
            translate(location)
                cylinder(r=r, h=shell_h +25);
        }
    }
}
// Centred cylinder in the x axis
module cylinder_x(r, len)
{
    rotate([0,90,0]) {
        cylinder(h=len, r=r, center=true);
    }
}

module axle_holes() {
    wheel_holes_y = [-12, 2, 24];
    shell_h_half = shell_h/2;
    mirror_x() {
        // These are holes for M3 bolts which will be the axles
        // (wheels have bearings)
        for (y = wheel_holes_y) {
            translate([shell_w_half, y, shell_h_half]) 
                cylinder_x(1.7, 20);
            // Need a cutout for the bolt heads
            translate([shell_w_half-1 - 3, y, shell_h_half]) 
                cylinder_x(4.0, 6);
            
        }
    }
}

module wiring_channels() {
    channel_h = 8;
    // Cutouts from one side of the gubbins section to the other
    // (for power wires etc)
    translate([-10,9,shell_h-12])
        fullybevelledbox([20,4,channel_h], radius=1.5);
    translate([-10,-22,shell_h-12])
        fullybevelledbox([20,4,channel_h], radius=1.5);
    
    // Cutouts from the gubbins to the rear (drive) motors
    // Which have their wiring near the centre.
    mirror_x() {
        translate([6,drive_motor_y + 2,shell_h-12])
            fullybevelledbox([4,8,channel_h], radius=1.5);
        
    }
    // Cutouts to allow wires to weapon motors
    // Which have their wiring very near the edge.
    mirror_x() {
        translate([shell_w_half-5,flip_motor_y-11,shell_h-7])
            fullybevelledbox([3,9,9], radius=1.5);
        translate([shell_w_half-8,flip_motor_y-12,shell_h-7])
            fullybevelledbox([6,4,9], radius=1.5);
        
    }    
}

module bottom_cutouts() 
{
    r = 3; // Radius of hex
    spacing = 6.0;
    min_x = 4.0 + r;
    max_x = shell_w_half - 10;
    min_y = -15;
    max_y = 11;
    $fn = 6; // cylinder -> hexagons
    mirror_x() {
        for (y= [min_y:spacing:max_y]) {
            for (x= [min_x:spacing:max_x]) {
                xhex = x + (((y - min_y) / 2) % spacing);
                if( (xhex> (min_x+2)) || (y< (max_y - 2))) {
                    translate([xhex , y, -0.5])
                        rotate([0,0,30])
                            cylinder(r=r, h=5.0);
                }
            }
        }
    }
}

module wedge_cutout()
{
    front_offset = 2.5;
    z_offset = 2.0;
    thickness = 1.5;
    difference() {
        intersection() {
            // Offset the front part of shell
            translate([0,0,- z_offset])
                rotate([0,0,90])
                rotate([90,0,0])
                    linear_extrude(height=shell_w - (thickness*2) , center=true) {
                        side_outline();
                    }
            // Cut off the front
            translate([-shell_w_half, shell_d_half - front_slope + front_offset, -1.0])
                cube([shell_w, front_slope, 50]);
        }
        // Solid part to support wedge by weapon
        cube([22,shell_d, shell_h*2 + 10], center=true);
    }
}

module axe_coupling() {
    // Show where the axe coupler will be
    rotate([0,90,0]) {
        union() {
            // Two flange plates = diameter 32mm,
            // thickness 2 mm.
            translate([0,0,1])
                cylinder(r=16.0, h=2.0);
            // The next part is 10mm diameter, 10mm high
            translate([0,0,3])
                cylinder(r=5.0, h=10.0);
        }
    }
}

module shell_main(skip_motors=false)
{
    difference() {
        rotate([0,0,90])
        rotate([90,0,0])
            linear_extrude(height=shell_w, center=true) {
                side_outline();
            }
        if (! skip_motors) {
            motor_cutouts();
        } else {
            // Motors skipped
            mirror_x() 
            translate([shell_w_half - 5, drive_motor_y,shell_h/2])
                cylinder_x(2.5, 20);
        }
        lid_cutout();
        other_cutouts();
        screw_holes();
        axle_holes();
        wiring_channels();
        bottom_cutouts();
        wedge_cutout();
    }
    translate([0,flip_motor_y,shell_h/2]) %axe_coupling();
}

shell_main();
