# Generate flat1_tinyholes.dxf with very small holes, for
# drilling out mechanically.


# We can laser etch the outline and the holes, so that the drill can be guided.
openscad flat1.scad -D override_hole_r=0.5 -o flat1_tinyholes.dxf

