include <defs.scad>;
include <utils.scad>;

$fs = 0.8;

hole_radius = 1.175;
corner_radius = 1.5;
plate_margin = 3.0; // must be more than hole_radius

// A plate with holes in

module four_holes() {
    // Like the holes in the motor mounts
    for (x=[-7,7]) {
        for (y=[-3, 3]) {
            translate([x,y])
                circle(hole_radius);
        }
    }
}

difference() {
    main_width = (wheel_x_rear - 21 + 6 +7 + plate_margin) *2;
    main_depth = 13 + (2* plate_margin);
    translate([0, - (main_depth / 2) + 3 + plate_margin])
        rounded_rect(main_width, main_depth, corner_radius);
    mirror_x() {
        union() {
            // Holes for drive motor mounts
            translate([wheel_x_rear - 21, 0])
                four_holes();
            // Weapon motor mounts
            translate([wheel_x_rear - 21 +7 , -7])
                four_holes();
        }
    }
}
