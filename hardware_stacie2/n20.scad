/*
 * N20 gear motor
 */

$fs = 0.4; // millimetres; smallest segment

motor_w = 16; // Width of motor (x axis)
motor_h = 10; // motor height
motor_d = 12; // y
gearbox_h = 10.6;
gearbox_d = 12;
gearbox_w = 10;
 
module n20_motor(margin=0.2)
{
    motor_d_half = motor_d / 2;
    motor_w_half = motor_w / 2;
    gearbox_w_half = gearbox_w / 2;
    // Oval motor body
    curve_rad = 10;
    translate([-motor_w_half - gearbox_w,0,0])
    rotate([0,90,0]) {
        $fa = 5.0; // more polygons?
        linear_extrude(motor_w, center=true) {
            intersection() {
            
                translate([0, motor_d_half - curve_rad - margin])
                    circle(r=curve_rad);
                translate([0, -motor_d_half + curve_rad + margin])
                    circle(r=curve_rad);
                square([motor_h,motor_d + (margin*2)], center=true);
            }
        }
    }
    // Cube gearbox
    translate([- gearbox_w_half, 0,0]) {
        cube([ gearbox_w + 0.1 + (margin*2), gearbox_d, gearbox_h], center=true); 
    }
    // Shaft
    translate([0,0,0]) {
        rotate([0,90,0]) {
            cylinder(h=10, r=1.5);
        }
    }
}

module n20_motor_top_ext(back_part=6.0) {
    // Motor with a box on top
    // And a cylinder out the back.
    n20_motor();
    translate([-motor_w-gearbox_w, -(motor_d/2), motor_h/2 - 0.2])
        cube([motor_w+gearbox_w, gearbox_d, gearbox_h]);
        
    // This is a cutout for wiring, terminals etc
    back_x = -motor_w-gearbox_w - back_part;
    translate([back_x, -(motor_d/2) + 1, -motor_h/2 + 2]) {
        cube([back_part + 0.2, motor_d - 2, motor_h + 0.2]);
    }
}

n20_motor();
