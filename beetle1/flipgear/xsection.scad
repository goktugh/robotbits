use <lid_gears.scad>;

projection(cut=true) {
    translate([0,0,-1])
        mirror([1,0,0])
            lidgear();
}
