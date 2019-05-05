use <shell.scad>;
use <MCAD/involute_gears.scad>;
include <inc/common.inc>;

$fs = 0.2; // millimetres
$fa = 4; // deg



module main() {
    gear(number_of_teeth=IDLE_COUNT,
        circular_pitch = CP, flat=true,
        bore_diameter=1);    
} 

main();
