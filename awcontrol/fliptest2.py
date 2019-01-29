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
        d0,d1 = 255-duty,255
    else:
        d1,d0 = 255-duty,255
    pigpio_f.write("p {} {}\n".format(GPIO_FLIP_0, d1))
    pigpio_f.write("p {} {}\n".format(GPIO_FLIP_1, d0))
    pigpio_f.flush()

def flipper_off():
    pigpio_f.write("w {} 0 w {} 0\n".format(GPIO_FLIP_0, GPIO_FLIP_1))
    pigpio_f.flush()

if __name__ == '__main__':
    init_pigpio()
    print("hello...")
    set_flipper(1, 128)
    time.sleep(0.1)
    set_flipper(1, 0)
    time.sleep(1.5)
    print("bye")
    flipper_off()
    


