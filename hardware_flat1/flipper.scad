include <defs.scad>;
use <utils.scad>;

// Set $fs (smallest arc fragment) to lower value than default 2.0
$fs = 0.8; // millimetres

/*
this will attach to both of the top motors, which have a 3mm
shaft with flat side,  the same as the wheel motors.

When in the "down" position, it needs to have a horizontal top, and
a diagonal section; it must not foul the wheels.

For this part, the origin will be in the centre of the axle, between
the two motors.

We will render this the correct orientation, but it probably isn't
printable that way.
*/

axle_diameter = 3.0;
axle_radius = axle_diameter / 2;
axle_radius_margin = 0.15; // Extra allowance for shrinkage etc

shaft_diameter = 12.0; // 
shaft_radius = shaft_diameter / 2;
shaft_length = 14.0; // between motors / gearboxes.

axle_nut_w = 5.0;
axle_nut_h = 1.0;
grub_screw_radius = 1.25;

flipper_width = 60;
horiz_length = 49.0; // y axis
thickness = 0.75;
bar_thickness = 2.0;
diagonal_1_length = 26;
diagonal_1_drop = 13;
diagonal_2_length = 16;
diagonal_2_drop = 16;

cutout_length = 24;
cutout_drop = 15;
ridge_radius = 1.0;

generate_flipper_main = true; // changed by -D

flipper_outline_points = [
            // Horizontal part of flipper
            [-shaft_radius, shaft_radius],
            [horiz_length, shaft_radius],
            // Diagonal part
            [horiz_length + diagonal_1_length, shaft_radius - diagonal_1_drop],
            [
                horiz_length + diagonal_1_length + diagonal_2_length, 
                shaft_radius - diagonal_1_drop - diagonal_2_drop
            ]
        ];

flipper_outline_points_1 = [
    flipper_outline_points[0],
    flipper_outline_points[1],
    flipper_outline_points[2],
    flipper_outline_points[3] + [-diagonal_2_length * 0.2, diagonal_2_drop * 0.2],
    ];

module flipper_outline(chop=0)
{
    $fn = 8;
    polyline(flipper_outline_points,
        thickness / 2); // radius
}


// An octahedron which is chopped at y=0, and only includes the negative
// part of y axis.
module octahedron_yminus(C0=1.0) {
    intersection() {
        octahedron(C0);
        translate([0,-C0,0])
            cube([C0 * 2, C0 * 2, C0 * 2], center=true);
    }
}

module flipper_reinforcement()
{
    // For more strength: ridge 
    translate([0, -(thickness / 2)])
    {
        // Centre line
        polyline_hulls(flipper_outline_points_1) {
                octahedron_yminus(ridge_radius);
        }
        
        mirror_z() {
            // edges
            edge_offset = (flipper_width / 2) - ridge_radius;
            translate([0,0,edge_offset])
                polyline_hulls(flipper_outline_points_1) {
                    octahedron_yminus(ridge_radius);
                };
            // Diagonals
            d1 = [
                flipper_outline_points_1[0],
                [ flipper_outline_points_1[1].x, flipper_outline_points_1[1].y, edge_offset],
                flipper_outline_points_1[2],
                [ flipper_outline_points_1[3].x, flipper_outline_points_1[3].y, edge_offset],
                ];
            polyline_hulls(d1) {
                octahedron_yminus(ridge_radius);
            };        
            d2 = [
                [ flipper_outline_points_1[0].x, flipper_outline_points_1[0].y, edge_offset],
                flipper_outline_points_1[1],
                [ flipper_outline_points_1[2].x, flipper_outline_points_1[2].y, edge_offset],
                flipper_outline_points_1[3],
                ];
            polyline_hulls(d2) {
                octahedron_yminus(ridge_radius);
            };        
        }
    }
}

module reinforcement_thing() {
    // Piece which reinforces the flipper
    // Centre piece
    linear_extrude(height=1.0, center=true) {
        intersection()
        {
            hull() {
                circle(r=shaft_radius - 0.5);
                translate([horiz_length, shaft_radius - 1.0]) {
                    circle(r=1.0);
                }
            }
            // Ensure that it does not fill the shaft hole
            translate([shaft_radius - 3, - shaft_radius])
                square([horiz_length+ shaft_radius, shaft_radius*2 + 1.0]);
        }
    }
}

module flipper_shaft_attachment()
{
   // This assumes that the shaft is in the z axis...
   // Shaft attachment
    linear_extrude(height=shaft_length, center=true, convexity=3) {
        difference() {
            union() {
                circle(shaft_radius);
                // square off the top side
                translate([0, shaft_radius / 2])
                    square([shaft_radius*2, shaft_radius], center=true);
                // Centre ridge piece
            }
            // Axle hole
            circle(axle_radius + axle_radius_margin);
            // Rectangle insert for nuts
            translate([0,1.0 + (axle_nut_h / 2)])
                square([axle_nut_w, axle_nut_h], center=true);

        }   
    }

}

module flipper_flipper()
{
    // Main flipper piece
    difference() {
        union()
        {
            // Flipper main
            linear_extrude(height=flipper_width, center=true, convexity=3) {
                flipper_outline();
            }
            flipper_reinforcement();
        }
        
        // Cutout top pieces.
        mirror_z() {
            cutout_z_len = ((flipper_width - shaft_length) / 2);
            translate([-shaft_radius - 10, shaft_radius - cutout_drop, (-flipper_width / 2) - 10])
                hull()
                {
                    translate([0,0,-cutout_z_len])
                    cube(
                        [cutout_length + 10, 
                        cutout_drop + 10, 
                        cutout_z_len + 10]);
                    translate([-cutout_length + (2* shaft_radius) - ridge_radius, 0,0])
                    cube(
                        [cutout_length + 10, 
                        cutout_drop + 10, 
                        cutout_z_len + 10]);
                }
        }
        // Cut off end
        translate([-shaft_radius - 10, 0,0])
            cube([20, shaft_radius * 3, flipper_width + 10], center=true);
    }
}

module flipper_screw_holes()
{
    // Need holes in the y direction, going down to y=0
    mirror_z() {
        rotate([-90,0,0]) {
            translate([0,4.0,0]) {
                cylinder(r=grub_screw_radius, h=30); // height does not matter.
            }
        }
    }
}

module flipper_main()
{
    rotate([0,90,0]) {
        rotate([0,0,90]) {
            difference() {
                union() {
                    flipper_shaft_attachment();
                    if (generate_flipper_main) {
                        flipper_flipper();
                    }
                    reinforcement_thing();
                }
                flipper_screw_holes();
            }
        }
    }
}

flipper_main();
