#!/usr/bin/env python3
import socket
import json
import time
import json

imu_socket = None

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
    for n in range(11):
        time.sleep(0.1)
        data = read_last_imu()
        yaws.append(data['yaw']) 
    print(repr(yaws))
    diffs = []
    for n in range(len(yaws)-1):
        diff = yaws[n+1] - yaws[n]
        diff = normalise_angle(diff)
        diffs.append(diff)
    print(repr(diffs))
    mean = sum(diffs) / len(diffs)
    mean *= 10 # degrees per second
    return mean

def main():
    init_socket()
    yaw_speed = get_yaw_speed()
    print("Yaw speed is {} degrees per second".format(yaw_speed))

if __name__ == '__main__':
    main()
