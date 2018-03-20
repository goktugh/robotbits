// Bevelled boxes, etc
module roundedbox(size=[1.0, 1.0, 1.0, ], radius = 0.1) {
    hull() {
        for (x = [radius, size[0] - radius]) {
            for (y = [radius, size[1] - radius]) {
                for (z = [radius, size[2] - radius]) {
                    translate([x,y,z])
                        sphere(r=radius);
                }
            }
        }
    }
}

module rounded_square_centered(size=[1,1], radius=0.1) {
    hull() {
        for (x = [- size[0] / 2 + radius, size[0] / 2 - radius]) {
            for (y = [- size[1] / 2 + radius, size[1] / 2 - radius]) {
                translate([x,y]) circle(radius);
            }
        }
    }
}

// Bevelled in x and y axes.
module bevelledbox(size=[1,1,1], radius=0.1) {
    linear_extrude(height=size[2]) {
        // 8-sided polygon
        polygon(points=[
            // Left
            [0, radius ],
            [0, size[1] - radius ],
            // Top
            [radius, size[1]],
            [size[0] - radius, size[1]],
            // Right
            [size[0], size[1] - radius],
            [size[0], radius],
            // Bot
            [size[0] -radius, 0],
            [radius, 0]
            ]);
    }
}

module bevelledbox_y(size=[1,1,1], radius=0.1) {
    // Bevelled around y
    translate([0,0,size[2]])
        rotate(-90, [1,0,0])
            bevelledbox([size[0], size[2], size[1]], radius); 
}

module bevelledbox_x(size=[1,1,1], radius=0.1) {
    // Bevelled around x
    translate([size[0],0,0])
        rotate(-90, [0,1,0])
            bevelledbox([size[2], size[1], size[0]], radius); 
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

module fullybevelledbox(size=[1,1,1], radius=0.1) {
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


