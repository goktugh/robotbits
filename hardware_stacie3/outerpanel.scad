include <inc/common.inc>;

$fs = 0.2; // millimetres

module outerpanel()
{
    wheel_holes_y = [1, -13, -35, drive_motor_y];
    r = 8;
    // Outer
    difference()
    {
        hull() {
            for (y = wheel_holes_y) {
                translate([0,y])
                    circle(r);
            }
        }
        // Wheel holes
        for (y = wheel_holes_y) {
            translate([0,y])
                circle(1.5);
        }
    }
}

outerpanel();
