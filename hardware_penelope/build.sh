#!/bin/bash
outdir=/media/sf_shared/penelope
set -x
mkdir -p $outdir

# 2d things - dxf
#for name in sidepanel outerpanel lid; do
#    openscad $name.scad -o $outdir/$name.dxf
#done

for name in shell; do

    openscad $name.scad -o $outdir/$name.stl
done

