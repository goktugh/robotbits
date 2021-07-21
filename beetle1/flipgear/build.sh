set -x
set -e
openscad -o xsection_motor.svg xsection_motor.scad
openscad -o xsection.svg xsection.scad
openscad -o only_lid_gear.amf only_lid_gear.scad
openscad -o only_motor_gear.amf only_motor_gear.scad

