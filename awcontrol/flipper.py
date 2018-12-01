#!/usr/bin/env python3
import time
import os

pigpio_f = None

GPIO_FLIP_0 = 19 # Enable to flip up
GPIO_FLIP_1 = 13 # enable to flip down

def init_pigpio():
    global pigpio_f
    pigpio_f = open('/dev/pigpio', 'wt')

def set_flipper(direction, duty):
    # Duty = 0...255
    if direction> 0:
        d0,d1 = duty,0
    else:
        d1,d0 = duty,0
    pigpio_f.write("p {} {}\n".format(GPIO_FLIP_0, d0))
    pigpio_f.write("p {} {}\n".format(GPIO_FLIP_1, d1))
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
    set_flipper(-1,32)
    time.sleep(0.6)
    print("end")
    set_flipper(0,0)

