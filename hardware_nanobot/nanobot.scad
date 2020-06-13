
front_axle_y = 13.0;
rear_axle_y = -13.0;


module motor_with_wheel()
{
    cylinder(r=3, h=16.5);
    translate([0,0,16.5]) 
        cylinder(r=1, h=3.5);
    // Wheel
    translate([0,0,12])
        %cylinder(r=6, h=8);
}

module motor_with_wheel_x()
{
    rotate([0,90,0])
        motor_with_wheel();
}

module main()
{
    translate([0, front_axle_y])
        motor_with_wheel_x();
    translate([0, rear_axle_y])
        motor_with_wheel_x();
    
}

main();
