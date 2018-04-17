use <flat1.scad>;
use <mount_plate.scad>;

union() {
    translate([0,0,5]) {
        linear_extrude(height=1) mount_plate_main();
    }
    color("darkgreen") 
        linear_extrude(height=1) main();

}
