#!/usr/bin/env python3
"""

"""
import socket
import json
import time
import os
import json
import math

from common import init_socket, init_pigpio, set_neutral, set_speeds, read_last_imu
import input_reader

SPEED_ZERO_THRESH = 0.01

P_FACTOR = 0.010 # Movement amount, per degree error
I_FACTOR = 0.010 # Movement amount, per degree-second integral error
D_FACTOR = 0.0004 # per degree per second error
I_CLAMP = 30.0 # Maximum
DEAD_ZONE = 0.1 # 
ROTATE_SPEED = 180 # Degrees per second, max

class Controller:
    deadzones = [] # list of lists, 
    target_yaw = None
    time_last = 0
    error_last = 0
    integral_error = 0
    tick_count = 0
    input_rotate = 0 # From controller, range -1.0 to 1.0 
    input_drive = 0 # From controller -1 ..1 
    
    def set_speeds(self, speed_l, speed_r):
        # set speeds, with speed_l and speed_r between -1.0 and 1.0 
        def clamp(v):
            return min(max(v,-1.0), 1.0)
        speed_l, speed_r = clamp(speed_l), clamp(speed_r)
        # Apply dead zone logic
        def speed_to_pwm(speed):
            if abs(speed) < SPEED_ZERO_THRESH:
                speed = 0
            else:
                # Apply the squaring rule,
                # To try to make it more proportional?
                speed *= abs(speed)
                if speed > 0:
                    speed = (speed * (1.0 - DEAD_ZONE)) + DEAD_ZONE
                else:
                    speed = (speed * (1.0 - DEAD_ZONE)) - DEAD_ZONE
            return int(speed * 255)
           
        set_speeds(speed_to_pwm(speed_l), speed_to_pwm(speed_r))
   
    def tick(self): 
        self.process_inputs()
        self.process_pid()

    def process_inputs(self):
        control_pos = input_reader.read_controller()
        if not control_pos.signal:
            self.target_yaw = None # Do not spin when no signal.
            self.integral_error = 0
        x = (control_pos.x / 127.0) 
        y = - (control_pos.y / 127.0) # y axis seems reversed.
        self.input_rotate = clamp(-1,1, x)
        self.input_drive = clamp(-1,1, y)

    def process_pid(self):
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
        self.target_yaw += self.input_rotate * ROTATE_SPEED * time_delta
        self.target_yaw = norm_angle(self.target_yaw)

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
        print("inputs: {:.2f} {:.2f} errors: P={:.2f} I={:.2f} D={:.2f}".format(self.input_rotate, self.input_drive, ang_error, self.integral_error, differential_error))
        # Calculate rotation amount:
        rot = (
            ang_error * P_FACTOR + 
            self.integral_error * I_FACTOR + 
            differential_error * D_FACTOR)
        # set forward speed depending on controller position and angle error.
        # More angle error = less drive speed, based on cos
        drive_speed = max(0.1, math.cos(math.radians(ang_error)))
        forward_speed = self.input_drive * drive_speed
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

def clamp(min_value, max_value, value):
    return min(max_value,max(min_value,value))

def main(): 
    try:
        init_socket()
        init_pigpio()
        cont = Controller()
        # Do a little dance
        i = read_last_imu()
        if i['motion']:
            raise Exception("Already moving")
        cont.set_speeds(0.5, 0.5)
        time.sleep(0.25)
        i = read_last_imu()
        if not i['motion']:
            raise Exception("Drive or IMU not working, cannot detect motion.")
        cont.set_speeds(0,0)
        time.sleep(1.0)
        cont.set_speeds(-0.5, -0.5)
        time.sleep(0.25)
        cont.set_speeds(0,0)
        time.sleep(1.0)
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
