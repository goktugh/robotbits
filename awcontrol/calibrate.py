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

def normalise_angle(a):
    if a > 180:
        a -= 360
    if a < -180:
        a += 360
    return a

def get_yaw_speed():
    yaws = []
    for n in range(6):
        if n > 0:
            time.sleep(0.1)
        data = read_last_imu()
        yaws.append(data['yaw']) 
    # print(repr(yaws))
    diffs = []
    for n in range(len(yaws)-1):
        diff = yaws[n+1] - yaws[n]
        diff = normalise_angle(diff)
        diffs.append(diff)
    # print(repr(diffs))
    mean = sum(diffs) / len(diffs)
    mean *= 10 # degrees per second
    return mean

def sample_yaws():
    time.sleep(0.5) ## alow it to reach speed
    # time for 5 periods, take middle value
    yaw_speeds = [get_yaw_speed() for n in range(5)]
    yaw_speed = sorted(yaw_speeds)[2]
    return yaw_speed

results = []

def try_pulse(ms):
    global results
    print("Trying {} ms".format(ms))
    set_speeds(ms, NEUTRAL_POS)
    left_speed = sample_yaws()
    print("Left yaw speed is {} degrees per second".format(left_speed))
    set_speeds(NEUTRAL_POS,ms)
    right_speed = sample_yaws()
    print("Right yaw speed is {} degrees per second".format(right_speed))
    results.append(
        ('left', ms, left_speed)
    )
    results.append(
        ('right', ms, right_speed)
    )
    
def main():
    init_socket()
    init_pigpio()
    set_neutral()
    os.makedirs('data', exist_ok=True)
    try: 
        for ms in range(1450, 1200, -10):
            try_pulse(ms)
        for ms in range(1450, 1700, 10):
            try_pulse(ms)
    finally:
        set_neutral()
    with open('data/calibrate0.json', 'wt') as f:
        results1 = list(sorted(results)) 
        json.dump(results1, f, indent=2)

if __name__ == '__main__':
    main()
