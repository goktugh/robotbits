#!/usr/bin/env python3
import socket
import json

def main():
    s = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    laddr = b'\0' + b'robot.IMU0'
    s.bind(laddr)
    while True:
        packet = s.recv(2048)
        data = json.loads(packet.decode('ascii'))
        print("y={:10} jerk={:10} t={:10}".format(data['yaw'], data['jerk'], data['time']))

if __name__ == '__main__':
    main()
