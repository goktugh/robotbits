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


def main(): 
    # Load deadzone file
    init_socket()
    init_pigpio()
    set_neutral()


if __name__ == '__main__':
    main()
