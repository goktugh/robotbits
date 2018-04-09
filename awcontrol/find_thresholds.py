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

from common import init_socket, init_pigpio, set_neutral, set_speeds, read_last_imu
from common import NEUTRAL_POS

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
    while (abs(bounds[1] - bounds[0]) > 2):
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
