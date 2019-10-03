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
    while True:
        print("Flipping...")
        set_flipper(1,96)
        time.sleep(0.1)
        set_flipper(0,0)
        time.sleep(0.5)
        print("retracting...")
        set_flipper(-1,64)
        time.sleep(0.2)
        set_flipper(-1,16)
        time.sleep(0.3)
        set_flipper(0,0)
        time.sleep(1.5)


