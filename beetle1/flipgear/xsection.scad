use <lid_gears.scad>;

projection(cut=true) {
    translate([0,0,-6])
        mirror([1,0,0])
            lidgear();
}
