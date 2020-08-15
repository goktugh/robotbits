use <shell.scad>;
use <base.scad>;
include <inc/common.inc>;

$fs = 0.6; // millimetres

// Shell
color("lightblue") shell_main();
base_main();
