/*
 * Toothed drive belt with the same pitch as the
 * wheel pulley.
 */
$fa = 0.5; // Minimum fragment angle

PI = 3.141592654;

WHEEL_RADIUS = 12;
WHEEL_THICKNESS = 6;
PULLEY_THICKNESS = 4.5;
PULLEY_RADIUS = 9.0;

/*
 * How long should the band / belt be?
 * 2*pi*PULLEY_RADIUS = 56.54 
 * 2* distance between pulleys 
    wheel_holes_y = [-6, -35];
    drive_motor_y = 23;
    
    23 - (-35) = 58 (distance between wheels)
    
 * 58*2 + 56.54 = 172.54 (total length of belt)
 
 * What should the diameter be?
 * 172.54 / pi = ~ 55
 */

band_length = 172.54;
band_radius = band_length / PI / 2;

belt_height = 3.2; 

// Blob_count is also 12
// So we have 12 teeth around the circumference of pulley_radius

tooth_pitch = (PULLEY_RADIUS * 2 * PI) / 12;
gap_width = (tooth_pitch * 0.75);
tooth_depth = 0.5;

tooth_count = ceil(band_length / tooth_pitch);

module drive_belt_main() {
    linear_extrude(belt_height) {
        difference() {
            // Outer
            circle(band_radius + 0.6);
            // centre cutout
            circle(band_radius - tooth_depth);
            //Teeth cutouts.
            for (n=[0:tooth_count]) {
                rotate([0,0,(n / tooth_count) * 360])
                    translate([band_radius - tooth_depth,0,0])
                        square([tooth_depth*2, gap_width], center=true);
            }
        }
    }
}

drive_belt_main();
