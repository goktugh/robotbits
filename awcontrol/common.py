#!/usr/bin/env python3
import socket
import json
import time
import os
import json
import select

imu_socket = None
pigpio_f = None

GPIO_LEFT = (18,23,24) # Enable, fwd, back
GPIO_RIGHT = (25,7,8) # Enable, fwd, back

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
    
def init_socket():
    global imu_socket
    s = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    laddr = b'\0' + b'robot.IMU0'
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
    return json.loads(bindata.decode('ascii'))

