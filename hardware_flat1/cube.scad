/*
 * Antweight cube. This should be 4 inches, with some tolerance.
 *
 * PRINT IN VASE MODE!
 */
 
tolerance = 0.6;

side = 101.6 + tolerance; // 4 inches

module cube_outside()
{
    cube([side, side, side],center=true);
    // perfectly flat sides do not work well. Add some extra bits, wiggles
    // Ribs
    for (ang = [0:90 :360]) {
        rotate([0,0,ang]) {
            for (x = [-side/4, 0, side/4]) {
                translate([x, side/2, 0]) {
                    rotate([0,0,45]) {
                        cube([1.5, 1.5, side], center=true);
                    }
                }
            }
        }
    }
}

module corner_cuts() {
    sqrt2 = sqrt(2);
    cornercut = 0.7;
    d = side/2 + (side/2/sqrt2) - cornercut;
    chunk = 2.0;
    for (z = [-side/2:chunk*2:side/2]) {
        for (ang = [0:90:360]) {
            rotate([0,0,ang]) {
                translate([d,d ,z]) {
                    rotate([0,0,45])
                        cube([side,side,chunk], center=true);
                }
            }
        }
    }
}

module bottom_cutouts() 
{
    r = 15; // Radius of hex
    $fn = 6; // cylinder -> hexagons
        
    translate([0,0, (-side/2) -0.5])
        rotate([0,0,30]) cylinder(r=r, h=200);
    for (ang = [0:60:360]) {
        rotate([0,0,ang]) {
            translate([r*2 + 2,0, (-side/2) -0.5])
                rotate([0,0,30]) cylinder(r=r, h=200);    
        }
    }
}

module main()
{
    difference(convexity=5) {
        cube_outside();
        corner_cuts();
        bottom_cutouts();
    }
}

main();

