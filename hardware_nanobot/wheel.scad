// $fa is related to arcs
// $fs = smallest segment
$fs = 0.4; // millimetres
$fa = 1.0;

shaft_length = 3;
shaft_diameter = 2.0;
flat_diameter = 1.5;

shaft_radius = shaft_diameter / 2.0;

wheel_outer_diameter = 10;
wheel_inner_diameter = wheel_outer_diameter - 1.0;
wheel_width = 8.0;
rim_width = 0.5;

module main()
{
    // shaft and base
    difference()
    {
        union() {
            // Shaft holder
            cylinder(r=shaft_radius + 1.0, h=shaft_length);
            // Base plate
            cylinder(r=(wheel_outer_diameter/2), h=rim_width);
        }
        // Shaft cutout with flat.
        translate([0,0,-0.5])
        linear_extrude(height=20)
            difference() {
                circle(r=shaft_radius);
                // flat
                translate([shaft_radius - (shaft_diameter - flat_diameter), -5])
                    square([10, 10]);
            }
    }
    // wheel outer
    linear_extrude(height=wheel_width)
    {
        difference() {
            circle(r=wheel_outer_diameter / 2);
            circle(r=wheel_inner_diameter / 2);
            // Small cutouts
            for (r=[0:10:360]) {
                rotate([0,0,r]) {
                    translate([(wheel_outer_diameter/2),0])
                        square([0.8,0.5], center=true);
                }
            } // endfor
        }
    }
    // Wheel top rim
    translate([0,0, wheel_width - rim_width])
    linear_extrude(height=rim_width)
    {
        difference() {
            circle(r=wheel_outer_diameter / 2);
            circle(r=wheel_inner_diameter / 2);
        }
    }
    
}

main();
