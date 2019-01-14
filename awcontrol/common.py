#!/usr/bin/env python3
import socket
import json
import time
import os
import json
import select

imu_socket = None
pigpio_f = None

GPIO_RIGHT = (25,7,8) # Enable, fwd, back
GPIO_LEFT = (18,24,23) # Enable, fwd, back

GPIO_FLIP_0 = 13 # Enable to flip up
GPIO_FLIP_1 = 19 # enable to flip down

def init_pigpio():
    global pigpio_f
    pigpio_f = open('/dev/pigpio', 'wt')

def set_speeds(l, r):
    # Range from -255 to 255
    def set_pulses(gpios, speed):
        en, fwd, back = gpios
        pulsef = max(speed,0)
        pulseb = max(-speed,0)
        pigpio_f.write("w {} 1 p {} {} p {} {}\n".format(en, fwd, pulsef, back, pulseb))

    # Left channel:
    set_pulses(GPIO_LEFT, l)
    set_pulses(GPIO_RIGHT, r)
    pigpio_f.flush()

def set_neutral():
    set_speeds(0,0)
    
def set_flipper(direction, duty):
    # Duty = 0...255
    if direction > 0:
        p0, p1 = duty,0
    else:
        p0, p1 = 0,duty

    pigpio_f.write("p {} {} p {} {}\n".format(GPIO_FLIP_0, int(p0), GPIO_FLIP_1, int(p1)))
    pigpio_f.flush()

def brake_flipper():
    pigpio_f.write("w {} 1 w {} 1\n".format(GPIO_FLIP_0, GPIO_FLIP_1))
    pigpio_f.flush()


def init_socket(unit=0):
    global imu_socket
    s = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    laddr = ('\0' + 'robot.IMU{}'.format(unit)).encode('ascii')
    s.bind(laddr)
    s.setblocking(0) # Make socket non-block
    imu_socket = s

def read_last_imu_bin():
    # Read data from the imu socket, and return the last message.
    # if there was none, then wait for one.
    last_packet = None
    while True:
        try:
            last_packet = imu_socket.recv(2048)
        except BlockingIOError:
            if last_packet:
                return last_packet 
            break
    # No last packet.
    timeout = 2.0
    select.select([ imu_socket], [], [], timeout)
    return imu_socket.recv(2048)

def read_last_imu():
    bindata = read_last_imu_bin()
    try:
        return json.loads(bindata.decode('ascii'))
    except ValueError:
        print("AARGH bad json from imu")
        print(bindata.decode('ascii'))
        raise

