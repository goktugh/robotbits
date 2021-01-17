
side_dimension = 101;

wall_thickness = 2.5;

module diamond(x,y) {
    polygon(
        points=[
            [x, 0], [0,-y],
            [-x,0], [0,y]
            ],
        paths=[ [0,1,2,3] ]
        );
}

module cutouts() {
    cutsize = side_dimension - 8.0;
    verybig = 200.0;
    holesize = 12.0;
    spacing = 6.0;
    diagspace = holesize * 2 + spacing;
    rotate([90,0,0])
    {
        linear_extrude(height=verybig, center=true, convexity=5)
        {
            for (y=[-1:1]) {
                for (x=[-1:1]) {
                    translate([x*diagspace,y*diagspace])
                        diamond(holesize, holesize);
                }
                if (y<1) {
                for (x=[-0.5:0.5]) {
                    translate([x*diagspace,(y+0.5)*diagspace])
                        diamond(holesize, holesize);
                }
                }
            }
        }
    }
}

module main()
{
    side_outer = side_dimension + wall_thickness*2;
    difference() {
        cube([side_outer, side_outer, side_dimension + wall_thickness],
            center=true);
        translate([0,0,wall_thickness])
            cube([side_dimension, side_dimension,side_dimension,], center=true);
        cutouts();
        rotate([0,0,90])
            cutouts();
    }
}

main();
