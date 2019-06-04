#!/bin/bash
outdir=/media/sf_shared/hardware
set -x
# 2d things - dxf
for name in sidepanel outerpanel idle_gear_2d lid; do
    openscad $name.scad -o $outdir/$name.dxf
done

for name in tyre_mould flip_coupler wheel_pulley wheel_pulley_bearing shell; do

    openscad $name.scad -o $outdir/$name.stl
done

