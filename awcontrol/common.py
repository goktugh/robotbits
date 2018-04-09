#!/usr/bin/env python3
import socket
import json
import time
import os
import json

imu_socket = None
pigpio_f = None

GPIO_RIGHT = 23
GPIO_LEFT = 24

NEUTRAL_POS = 1450 # milliseconds

def init_pigpio():
    global pigpio_f
    pigpio_f = open('/dev/pigpio', 'wt')

def set_speeds(l, r):
    pigpio_f.write("s {} {}\n".format(GPIO_LEFT, l))    
    pigpio_f.write("s {} {}\n".format(GPIO_RIGHT, r))    
    pigpio_f.flush()

def set_neutral():
    set_speeds(NEUTRAL_POS, NEUTRAL_POS)
    
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
    # No last packet.
    timeout = 5.0
    select.select([ imu_socket], [], [], timeout)
    return imu_socket.recv(2048)

def read_last_imu():
    bindata = read_last_imu_bin()
    return json.loads(bindata.decode('ascii'))

