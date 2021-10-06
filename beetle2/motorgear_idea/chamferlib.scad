
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

module chamferredbox(size=[1,1,1], radius=0.1) {
    hull() {
        for (x = [radius, size[0] - radius]) {
            for (y = [radius, size[1] - radius]) {
                for (z = [radius, size[2] - radius]) {
                    translate([x,y,z])
                        octahedron(radius);
                }
            }
        }
    }
}

