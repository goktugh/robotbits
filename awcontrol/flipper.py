#!/usr/bin/env python3
import time
import os

pigpio_f = None

GPIO_FLIP_ENABLE = 19
GPIO_FLIP_0 = 6 # Enable to flip up
GPIO_FLIP_1 = 13 # enable to flip down

def init_pigpio():
    global pigpio_f
    pigpio_f = open('/dev/pigpio', 'wt')

def set_flipper(direction, duty):
    # Duty = 0...255
    p0 = (direction > 0)
    p1 = (direction < 0)
    pigpio_f.write("p {} {}\n".format(GPIO_FLIP_ENABLE, int(duty)))
    pigpio_f.write("w {} {}\n".format(GPIO_FLIP_0, int(p0)))
    pigpio_f.write("w {} {}\n".format(GPIO_FLIP_1, int(p1)))
    pigpio_f.flush()


if __name__ == '__main__':
    init_pigpio()
    print("Flipping...")
    set_flipper(1,255)
    time.sleep(0.10)
    print("waiting...")
    set_flipper(0,0)
    time.sleep(0.2)
    print("retracting...")
    # Retract slowly.
    set_flipper(-1,64)
    time.sleep(1)
    print("holding...")
    # Retract slowly.
    set_flipper(-1,32)
    time.sleep(1.0)
    print("end")
    set_flipper(0,0)

