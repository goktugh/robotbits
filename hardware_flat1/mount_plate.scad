include <inc/defs.scad>;
include <inc/utils.scad>;

$fs = 0.8;

hole_radius = 1.25;
corner_radius = 1.5;
plate_margin = 3.0; // must be more than hole_radius

// A plate with holes in

module motor_holes(rows=2, cols=3) {
    // Like the holes in the motor mounts
    for (x=[-3:6: (6*cols -9)]) {
        y_coords = (rows == 2) ?  [-7,7] : [-7];
        for (y=y_coords) {
            translate([x,y])
                circle(hole_radius);
        }
    }
}

module holes_all()
{
    // Front and rear holes (12 at rear, 4 at front)
    translate([17,wheel_y_rear,0])
        motor_holes();
    translate([17,wheel_y_front,0]) 
        motor_holes(rows=1, cols=2);
}


module mount_plate_main() {
    mirror_x() {
        // Rear and front
            difference() {
                hull() {
                    minkowski() {
                        holes_all();
                        circle(1.5);
                    }
                }
                holes_all();
            }
        // Connectors
        midpoint_y = (wheel_y_front + wheel_y_rear) / 2;
        polyline([
                [-12, wheel_y_rear + 10 ],
                [12, wheel_y_front - 10 ],
                [-12, wheel_y_front - 10 ],
                [-12, midpoint_y ],
                [12, midpoint_y ],
            ], 
            0.5); // Radius
    }
}

// this is for the flipper motor to lie flat.
module filled_region() {
    // LH side.
   translate([-17 - 6,wheel_y_rear,0])
   difference() {
        hull() {
            offset(r=1.5) {
                motor_holes();
            }
        }
        motor_holes();
    }
    // rh side
    difference() {
        for (y=[wheel_y_rear + 7, wheel_y_rear - 7] ) {
            polyline([
                [14, y],
                [26, y],
                ], 2);
        }
        translate([17,wheel_y_rear])
            motor_holes();
    }
    // Front
    mirror_x() {
        difference() {
            polyline([
                [14, wheel_y_front - 7],
                [20, wheel_y_front - 7],
                ], 2);
            translate([17,wheel_y_front])
                motor_holes();
        }
    }
}

main();
