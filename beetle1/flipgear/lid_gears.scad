use <gears.scad>;
use <chamferlib.scad>;

tooth_modulus = 2.4;
pressure_angle = 20;
helix_angle = 30;

gears_thickness = 20.0;
lidgear_thickness = 16.0;

module motorgear() {
    num_teeth = 13;
    splodgefactor = 0.25; // Diam.
    /* Herringbone_gear; uses the module "spur_gear"
    modul = Height of the Tooth Tip beyond the Pitch Circle
    tooth_number = Number of Gear Teeth
    width = tooth_width
    bore = Diameter of the Center Hole
    pressure_angle = Pressure Angle, Standard = 20° according to DIN 867. Should not exceed 45°.
    helix_angle = Helix Angle to the Axis of Rotation, Standard = 0° (Spur Teeth)
    optimized = Holes for Material-/Weight-Saving */

    bore = 0; // Diameter (not radius) of centre hole
    difference() {
        herringbone_gear (tooth_modulus, num_teeth, gears_thickness, bore, 
            pressure_angle=pressure_angle, helix_angle=helix_angle);
        // Hex cutout - 8mm across flat
        across_flat = 8.6; // Allow for splodge
        cos_30 = 0.866;
        across_edge = across_flat / cos_30; 
        translate([0,0,-1]) cylinder($fn=6, r=(across_edge/2), h=30);
        // Cutout for set-screw
        translate([-3,0,-14]) chamferredbox(size=[6,40,20], radius=2.0);
    }
}


lidgear_teeth = 55;
lidgear_angle = 120;
lidgear_thickness_offset = 12; // mm centre is above the lid, hinge + thickness.
lidgear_width_offset = 15; // distance from hinge

module lidgear() {
    num_teeth = lidgear_teeth;
    bore = 24.0; // Diameter of bore (not radius)

    // color("green") translate([lidgear_thickness_offset,-lidgear_width_offset,0] ) {
    //     cylinder(r=1, h=30);
    // }
    
    centre_z = (gears_thickness /2);
    
    difference() {
        union() {            
            intersection() {
                translate([lidgear_thickness_offset,-lidgear_width_offset,(gears_thickness-lidgear_thickness) /2])
                herringbone_gear (tooth_modulus, num_teeth, lidgear_thickness, bore, 
                    pressure_angle=pressure_angle, helix_angle=-helix_angle,
                    optimized=false);
                a1 = 90 - (lidgear_angle/2);
                // Cut off the parts we do not need.
                rotate([0,0,0]) 
                    translate([-100,0,0]) cube([200,200,200], center=true);
                rotate([0,0,-a1*2]) 
                    translate([-100,0,0]) cube([200,200,200], center=true);
            }
            // Side panels
            panel_y_size = 36;
            panel_z_size = 60;
            translate([-10,5,centre_z-(panel_z_size/2)]) {
                chamferredbox(size=[10,panel_y_size,panel_z_size], radius = 2.0);
            }
        }
        // Screw holes....
        translate([0, -(lidgear_width_offset/2), 0])
        {
            for (n=[20,40]) {
                translate([0,n,gears_thickness/2])
                thread_insert_cutout();
            }
            // Cutouts at the side, -z and +z
            for (z=[-20,20]) {
                translate([0,40,gears_thickness/2 + z])
                thread_insert_cutout();
            }
            // Indicator hole
            translate([0,40,0])
                cylinder(r=0.5, h=80, center=true);
            // Cutout for general weight-saving
            /*
            translate([0,0,-1])
            linear_extrude(100) {
                intersection() {
                    circle(r=40);                
                    translate([-10,10])
                        mirror([1,0])
                        square([100,100]);
                };
            }
            */
        }
        
        // Indicator grooves.
        /*
        translate([lidgear_thickness_offset,-lidgear_width_offset,
                (gears_thickness+lidgear_thickness)/2 + 0.3]) {
            for (n=[15,30,45,60,75]) {
                rotate([0,0,n]) 
                    rotate([90,0,0]) 
                        cylinder($fn=4, r=1, h=200, center=true);
            }
        }
        */
        // Cutouts to improve strength
        // NB this piece is in the -X direction of the axis.
        translate([lidgear_thickness_offset,-lidgear_width_offset])
        for (n=[0,20,40]) {
            rotate([0,0,n+120])
            translate([30,0]) cube([30,1,gears_thickness]);
        }
    }
}

module thread_insert_cutout()
{
    lid_hole_radius = 2.75;
    chamfer = 2.5;
    rotate([0,90,0])
        union() {
            cylinder(r=lid_hole_radius, h=35, center=true);
            cylinder(r1=lid_hole_radius, r2=lid_hole_radius+chamfer, h=chamfer, center=true);
        }
}

centre_distances = 96.0;

module main() {
    translate([-centre_distances,0,0]) motorgear();
    lidgear();
}

main();
