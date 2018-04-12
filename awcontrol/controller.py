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

SPEED_ZERO_THRESH = 0.01
PULSE_WIDTH_FULL = 400 # microseconds, offset from dead zone
PULSE_WIDTH_SMALL = 5 # Offset from dead zone

P_FACTOR = 0.01 # Movement amount, per degree error
I_FACTOR = 0.0 # Movement amount, per degree-second integral error
D_FACTOR = 0.00001 # per degree per second error
I_CLAMP = 10.0 # Maximum

class Controller:
    deadzones = [] # list of lists, 
    target_yaw = None
    time_last = 0
    error_last = 0
    integral_error = 0
    tick_count = 0
    
    def load_deadzones(self):
        # Load deadzone file
        with open('data/deadzone.txt', 'rt') as f:
            self.deadzones = []
            # Read left, then right
            for n in range(2):
                line = f.readline()
                bits = line.split()[1:] # Ignore first element
                self.deadzones.append(tuple(map(int, bits[:2])))

    def set_speeds(self, speed_l, speed_r):
        # set speeds, with speed_l and speed_r between -1.0 and 1.0 

        def get_pulse(speed, deadzones):
            # clamp speed
            speed = min(speed, 1.0)
            speed = max(speed, -1.0)
            if abs(speed) < SPEED_ZERO_THRESH:
                return NEUTRAL_POS
            if speed > 0:
                return deadzones[1] - PULSE_WIDTH_SMALL + (PULSE_WIDTH_FULL * speed)
            else: # Less than zero: subtract from deadzones[0]
                return deadzones[0] + PULSE_WIDTH_SMALL + (PULSE_WIDTH_FULL * speed)
   
        # Invert left channel because it's wired that way. 
        pulse_l = get_pulse(- speed_l, self.deadzones[0])
        pulse_r = get_pulse(speed_r, self.deadzones[0])
       
        set_speeds(pulse_l, pulse_r) 
   
    def tick(self): 
        # Will wait for the next imu data, if one is not ready.
        self.imu_data = read_last_imu() 
        yaw = self.imu_data['yaw']
        if self.target_yaw is None:
            self.target_yaw = yaw
        time_now = (self.imu_data['time'] / 1000.0)  # time in secs
        # Store last and current time, so we can calc integral error.
        # Integral error will be in degree-seconds
        if self.time_last == 0:
            self.time_last = time_now
        time_delta = time_now - self.time_last
        # Avoid division by zero, by making time_delta always something...
        time_delta = max(time_delta, 0.001)

        # Calculate angular error,
        ang_error = norm_angle(yaw - self.target_yaw)
        # Integral error
        self.integral_error += ang_error * time_delta
        # Clamp integral error to maximum
        self.integral_error = min(self.integral_error, I_CLAMP)
        self.integral_error = max(self.integral_error, - I_CLAMP)
        # Differential error
        differential_error = (ang_error - self.error_last) / time_delta

        # Calculate P and I
        # P is the proportional (angdiff)
        # I is the integral
        print("errors: P={:.2} I={:.2} D={:.2}".format(ang_error, self.integral_error, differential_error))
        # Calculate rotation amount:
        rot = (
            ang_error * P_FACTOR + 
            self.integral_error * I_FACTOR + 
            differential_error * D_FACTOR)
        # Make a nice forward-backward pattern.
        forward_speed = 0.0
        # t = (self.tick_count // 40) % 4
        # if t == 1:
        #     forward_speed = 0.2
        # if t == 3:
        #    forward_speed = -0.2

        # Drive to rot + forward speed
        self.set_speeds(-rot + forward_speed, rot +forward_speed) 
        
        self.error_last = ang_error
        self.time_last = time_now
        self.tick_count += 1
    

def norm_angle(a):
    if a > 180:        
        return a - 360
    if a < -180:        
        return a + 360
    return a

def main(): 
    try:
        init_socket()
        init_pigpio()
        cont = Controller()
        cont.load_deadzones()
        # Do a little dance
        i = read_last_imu()
        if i['motion']:
            raise Exception("Already moving")
        cont.set_speeds(0.1, 0.1)
        time.sleep(0.5)
        i = read_last_imu()
        if not i['motion']:
            raise Exception("Drive or IMU not working, cannot detect motion.")
        cont.set_speeds(0,0)
        time.sleep(1.0)
        cont.set_speeds(-0.1, -0.1)
        time.sleep(0.5)
        cont.set_speeds(0,0)
        # Done
        while True:
            cont.tick()
    finally:
        try:
            set_neutral()
        except Exception as e:
            pass # Ignore now

if __name__ == '__main__':
    main()
