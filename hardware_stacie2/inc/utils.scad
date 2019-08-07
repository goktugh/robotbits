
// 2d polyline, circles connected by segments.
// Points is an array of arrays with a minimum of 2 elements.
module polyline(points, radius)
{
    polyline_hulls(points) {
        circle(radius);
    }
}

// Do a polyline, by making hulls between each pair of points
// and duplicating children() at those points...
module polyline_hulls(points) {
    for (i=[0:len(points)-2]) {
        p1 = points[i];
        p2 = points[i+1];
        hull() {
            translate(p1) children();
            translate(p2) children();
        }
    }    
}

module mirror_x()
{
    union() {
        children();
        mirror([1,0,0]) children();
    }
}

module mirror_y()
{
    union() {
        children();
        mirror([0,1,0]) children();
    }
}

module mirror_z()
{
    union() {
        children();
        mirror([0,0,1]) children();
    }
}

// Draw origin-centred, unit polyhedron.
module octahedron(C0=1.0) {
    polyhedron(points = [
        [0.0, 0.0,  C0],
        [0.0, 0.0, -C0],
        [ C0, 0.0, 0.0],
        [-C0, 0.0, 0.0],
        [0.0,  C0, 0.0],
        [0.0, -C0, 0.0]
        ],
        faces = [
[ 4 , 2, 0],
[ 3 , 4, 0],
[ 5 , 3, 0],
[ 2 , 5, 0],
[ 5 , 2, 1],
[ 3 , 5, 1],
[ 4 , 3, 1],
[ 2 , 4, 1]
        ]
    );
}

module diamond(C0=1.0) {
    rotate([0,0,45])
        square([C0 * sqrt(2), C0 * sqrt(2)], center=true);
}

// Rounded rectangle, centred.
module rounded_rect(w,h,r)
{
    hull() {
        translate([(-w / 2) + r, (-h / 2) + r])
            circle(r);
        translate([(w / 2) - r, (-h / 2) + r])
            circle(r);
        translate([(-w / 2) + r, (h / 2) - r])
            circle(r);
        translate([(w / 2) - r, (h / 2) - r])
            circle(r);
    }
}

