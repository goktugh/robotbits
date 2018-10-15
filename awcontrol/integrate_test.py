#!/usr/bin/env python3

from common import init_socket, read_last_imu

def read_accel():
    i = read_last_imu()
    ax,ay,az = i['ax'], i['ay'], i['az']
    return ax,ay,az
    
def main():
    init_socket(1)
    # Assume we are still now.
    # take some readings to determine gtravity vector
    for n in range(10):
        a = read_accel()

    while True:
        i = read_last_imu()
        ax,ay,az = i['ax'], i['ay'], i['az']
        print("{:f} {:f} {:f}".format(ax,ay,az))

if __name__ == '__main__':
    main()
