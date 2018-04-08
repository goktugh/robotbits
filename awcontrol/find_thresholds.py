#!/usr/bin/env python3
"""

This program finds the dead zone, the region where the motors
do not run.

This is a range of pulse lengths, and quite a large range.

"""
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

def check_movement(channelindex, pulse):
    pulses = [NEUTRAL_POS, NEUTRAL_POS]
    pulses[channelindex] = pulse
    set_speeds(*pulses) 
    # Wait a short time 
    time.sleep(1.5)
    # set it back to neutral
    set_neutral()
    imu_data = read_last_imu()
    moves = imu_data['motion'] 
    print("channel={} pulse={}, moves={}".format(channelindex, pulse, moves))
    time.sleep(1.0)
    # Check that we really don't have movement in neutral
    imu_data = read_last_imu()
    if imu_data['motion']:
        raise Exception("Still getting movement in neutral pos")
    return moves

def find_thresh(channelname, channelindex):
    print("find_thresh: {} {}".format( channelname, channelindex))
    # Find lower threshold of movement
    print("Finding low bound")
    bounds = [1300, 1450]  # Initial bounds
    refine_bounds(channelindex, bounds)
    low_thresh = int(sum(bounds) / 2)
    print("Finding high bound")
    bounds = [1600, 1450]  # Initial bounds
    refine_bounds(channelindex, bounds)
    high_thresh = int(sum(bounds) / 2)
    print("Dead zone: {} {}".format(low_thresh, high_thresh))
    return low_thresh, high_thresh

def refine_bounds(channelindex, bounds):
    # we know that it will not move at bounds[1]
    # But it will move at bounds[0]
    # So we will try the middle value
    while (abs(bounds[1] - bounds[0]) > 5):
        middle = int(sum(bounds) / 2)
        does_move = check_movement(channelindex, middle)
        # Move the boundaries accordingly.
        if does_move:
            bounds[0] = middle
        else:
            bounds[1] = middle
    print("Found bounds at " + repr(bounds))

def main():
    init_socket()
    init_pigpio()
    set_neutral()
    os.makedirs('data', exist_ok=True)
    dzleft = find_thresh("left", 0)
    dzright = find_thresh("right", 1)
    with open('data/deadzone.txt', 'wt') as f:
        print("left {} {}".format(*dzleft), file=f)
        print("right {} {}".format(*dzright), file=f)

if __name__ == '__main__':
    main()
