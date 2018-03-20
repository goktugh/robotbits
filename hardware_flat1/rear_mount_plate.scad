include <inc/defs.scad>;
include <inc/utils.scad>;

$fs = 0.8;

hole_radius = 1.175;
corner_radius = 1.5;
plate_margin = 3.0; // must be more than hole_radius

// A plate with holes in

module four_holes() {
    // Like the holes in the motor mounts
    for (x=[-3,3]) {
        for (y=[-7, 7]) {
            translate([x,y])
                circle(hole_radius);
        }
    }
}

difference() {
    main_width = 60;
    main_depth = 28;
    translate([0, 0])
        rounded_rect(main_width, main_depth, corner_radius);
    mirror_x() {
        union() {
            // Holes for drive motor mounts
            translate([wheel_x_rear - 21, 3.5])
                four_holes();
            // Weapon motor mounts
            translate([wheel_x_rear - 21 +7 , -3.5])
                four_holes();
        }
    }
}
