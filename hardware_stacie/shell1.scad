use <n20.scad>;
use <inc/utils.scad>;

$fs = 0.6; // millimetres

shell_w = 70;
shell_d = 90;
shell_h = 14;

inner_w = 30;
inner_d = 86;
thickness_bot = 1.0;
motor_y = (shell_w / 2) - 5;
thickness2 = 2.0;

bolt_radius = 1 + 0.15; // M2 bolts

module mirror_xy() {
    mirror_x() {
        mirror_y() {
            children();
        }
    }
}

module main()
{
    difference() {
        cube([shell_w, shell_d, shell_h], center=true);
        // Cutout for motor
        mirror_x() {
            translate([shell_w / 2, motor_y,0]) {
                n20_motor();
            }
        }
        // Cut the middle out for gubbins
        translate([0,0,thickness_bot])
            cube([inner_w, inner_d, shell_h], center=true);
        // Cut out more...
        translate([0,-8,thickness_bot])
            cube([shell_w - thickness2, shell_d - thickness2 - 30, shell_h], center=true);

        // Cut the top off, we will put a lid here
        lid_height = 3.0;
        translate([0,0,(shell_h / 2) - (lid_height/2) + 0.1]) 
            cube([shell_w -2, shell_d - 2, lid_height], center=true);
        // Bolt holes
        mirror_xy() {
            translate([shell_w / 2 - 4, shell_d / 2 - 4, 0])
                cylinder(r=bolt_radius, h=20, center=true);
        }
    }
}

main();
