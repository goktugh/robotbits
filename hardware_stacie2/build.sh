#!/bin/bash
outdir=/media/sf_shared/hardware
set -x
for name in idle_gear wheel_gear wheel_gear_bearing shell; do

    openscad $name.scad -o $outdir/$name.stl
done

