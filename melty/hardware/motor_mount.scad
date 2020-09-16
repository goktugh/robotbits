$fs = 0.6; // millimetres
$fa = 4;

width=44;
height=14;
corner=2;

kerf = 0.15;
centre_rad = 1.5 - kerf;
outer_rad = 1.0 - kerf;
hole_spacing = 36.0;

module main()
{
    // outline
    difference()
    {
        hull() {
            translate([corner, corner]) circle(corner);
            translate([width-corner, corner]) circle(corner);
            translate([corner, height-corner]) circle(corner);
            translate([width-corner, height-corner]) circle(corner);
        }
        //  Holes
        translate([width/2, height/2]) {
            circle(centre_rad);
            // Motor mount holes are 10mm spaced.
            rotate([0,0,45]) {
                translate([-5,0]) circle(outer_rad);
                translate([5,0]) circle(outer_rad);
                translate([0,5]) circle(outer_rad);
                translate([0,-5]) circle(outer_rad);
            }
            // screw holes
            translate([-hole_spacing/2, 0]) 
                circle(outer_rad);
            translate([hole_spacing/2, 0]) 
                circle(outer_rad);
        }
    }
}

main();
