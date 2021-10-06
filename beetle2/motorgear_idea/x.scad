use <gears.scad>;
use <chamferlib.scad>;

tooth_modulus = 2.4;
pressure_angle = 20;
helix_angle = 30;

gears_thickness = 32.0;

module motorgear() {
    num_teeth = 15;
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
        // Hex cutout - *WAS* 8mm across flat
        // Now 12.7 across flat.
        across_flat = 12.7 + 0.4; // Allow for splodge
        cos_30 = 0.866;
        across_edge = across_flat / cos_30; 
        translate([0,0,-1]) cylinder($fn=6, r=(across_edge/2), h=gears_thickness+5);
        // Cutout for set-screw
        translate([-3,0,-14]) chamferredbox(size=[6,40,20], radius=2.0);
        // Cutout for the motor itself
        motor_diameter = 25.0;
        cutout_diameter = motor_diameter + 2.0;
        cutout_radius = cutout_diameter/2; 
        translate([0,0,gears_thickness/2])
            cylinder(r=cutout_radius, h=gears_thickness);
    }
}


module main() {
 motorgear();
}

main();
