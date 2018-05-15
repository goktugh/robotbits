#!/usr/bin/env python3
import struct
import os
import time
import collections

InputEvent = collections.namedtuple('InputEvent', ['tv_sec', 'tv_usec','type', 'code', 'value'])

# Constants from linux/input-event-codes.h

EV_SYN=0
EV_KEY=1
EV_REL=2
EV_ABS=3

# Our axes defined in input-event-codes. These seem to be mostly wrong.
ABS_X   =0x00
ABS_Y   =0x01
ABS_Z   =0x02
ABS_RX  =0x03
ABS_RY  =0x04
ABS_RZ  =0x05

AXES_X = (0,2)
AXES_Y = (1,3)
# Buttons for "flip" - X, and all the shoulder buttons.
BUTTONS_FLIP =  (0x12e, 0x128, 0x129, 0x12a, 0x12b)

input_dev = '/dev/input/event0'

#long int, long int, unsigned short, unsigned short, unsigned int
FORMAT = 'llHHi'
EVENT_SIZE = struct.calcsize(FORMAT)

def read_event(fd):
    d = fd.read(EVENT_SIZE)
    if d is None:
        return None
    fields = struct.unpack(FORMAT, d)
    return InputEvent(*fields)
   
def main(): 
    fd = open(input_dev, 'rb')
    os.set_blocking(fd.fileno(), 0)
    while True:
        while True:
            e = read_event(fd)
            if e:
                if e.type == EV_ABS and e.code in (ABS_X, ABS_Y, ABS_Z, ABS_RX, ABS_RY, ABS_RZ):
                    print("abs {:04x} {}".format(e.code, e.value))
                if e.type == EV_KEY:
                    print("key {:04x} {}".format(e.code, e.value))
            else:
                break
        time.sleep(0.1)
        print("Tick")

if __name__ == '__main__':
    main()    
