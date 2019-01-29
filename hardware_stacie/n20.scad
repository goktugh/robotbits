/*
 * N20 gear motor
 */

$fs = 0.4; // millimetres; smallest segment
 
module n20_motor()
{
    motor_w = 12; // Width of motor (x axis)
    motor_h = 10; // motor height
    motor_d = 12;
    motor_d_half = motor_d / 2;
    motor_w_half = motor_w / 2;
    gearbox_w = 10;
    gearbox_w_half = gearbox_w / 2;
    gearbox_h = 12;
    gearbox_d = 12;
    // Oval motor body
    curve_rad = 10;
    translate([-motor_w_half - gearbox_w,0,0])
    rotate([0,90,0]) {
        $fa = 5.0; // more polygons?
        linear_extrude(motor_w, center=true) {
            intersection() {
            
                translate([0, motor_d_half - curve_rad])
                    circle(r=curve_rad);
                translate([0, -motor_d_half + curve_rad])
                    circle(r=curve_rad);
                square([motor_h,motor_d], center=true);
            }
        }
    }
    // Cube gearbox
    translate([- gearbox_w_half, 0,0]) {
        cube([ gearbox_w + 0.1, gearbox_d, gearbox_h], center=true); 
    }
    // Shaft
    translate([0,0,0]) {
        rotate([0,90,0]) {
            cylinder(h=10, r=1.5);
        }
    }
    

}


n20_motor();
