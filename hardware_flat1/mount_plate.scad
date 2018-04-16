include <inc/defs.scad>;
include <inc/utils.scad>;

$fs = 0.8;

hole_radius = 1.175;
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

module main() {
    mirror_x() {
        // Rear...
        translate([wheel_x_rear - 21 + 7,wheel_y_rear,0]) 
            difference() {
                hull() {
                    minkowski() {
                        motor_holes();
                        circle(2.0);
                    }
                }
                motor_holes();
            }
        // Front...
        translate([wheel_x_front - 21 + 7,wheel_y_front,0]) 
            difference() {
                hull() {
                    minkowski() {
                        motor_holes(rows=1, cols=2);
                        circle(2.0);
                    }
                }
                motor_holes(rows=1, cols=2);
            }
        // Connectors
        polyline([
                [20, wheel_y_rear + 10],
                [20, wheel_y_front - 10],
                [-20, wheel_y_rear + 10 ],
                [33, wheel_y_rear + 10 ],
                [20, wheel_y_front - 10 ],
                // [20, wheel_y_rear + 10 ],
                [28, wheel_y_front - 10 ],
                [34, wheel_y_rear + 10 ],
            ], 
            1); // Radius
    }
}

main();
