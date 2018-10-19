#!/usr/bin/env python3
"""

"""
import socket
import json
import time
import os
import json
import math

from common import init_socket, init_pigpio, set_neutral, set_flipper, set_speeds, read_last_imu
import input_reader

SPEED_ZERO_THRESH = 0.01

P_FACTOR = 0.005 # Movement amount, per degree error
I_FACTOR = 0.020 # Movement amount, per degree-second integral error
D_FACTOR = 0.0005 # per degree per second error
I_CLAMP = 30.0 # Maximum
DEAD_ZONE = 0.05 #  amount of pwm which does not have any effect
ROTATE_SPEED = 180 # Degrees per second, max

DRIVE_SCALE = 0.3 # scaling factor for forward/back drive
DRIVE_SCALE_FAST = 0.6 # when driving fast
FAST_TIME = 0.1 # drive fast for this long after stop

# Flip sate map: direction, duty, time to next state, next state
FLIP_STATE_MAP = {
    'idle': (0,0,0, 'idle'),
    'flip': (1, 255, 0.15, 'hold'),
    'hold': (0, 0, 0.15, 'retract'),
    'retract': (-1, 80, 0.25, 'retract2'),
    'retract2': (-1, 32, 0.15, 'idle'),
    # Special state for quick retraction.
    'retract3': [-1, 255, 0.15, 'idle'],
}

class Controller:
    deadzones = [] # list of lists, 
    target_yaw = None
    time_last = 0
    error_last = 0
    integral_error = 0
    tick_count = 0
    input_rotate = 0 # From controller, range -1.0 to 1.0 
    input_drive = 0 # From controller -1 ..1 
    input_flip = False
    input_flip_up = False
    input_flip_down = False
    flip_state = 'idle'
    flip_timeout = 0 # Time left to next state
    retract_time = 0 # If >0, then retract
    last_stop_time = 0 # Last time the stop button was pressed
    last_nodrive_time = 0 # Last time when we were not driving fwd or back.
    
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
                if speed > 0:
                    speed = (speed * (1.0 - DEAD_ZONE)) + DEAD_ZONE
                else:
                    speed = (speed * (1.0 - DEAD_ZONE)) - DEAD_ZONE
            return int(speed * 255)
           
        set_speeds(speed_to_pwm(speed_l), speed_to_pwm(speed_r))
   
    def tick(self): 
        self.process_inputs()
        self.process_pid()
        self.process_flipper()

    def process_inputs(self):
        control_pos = input_reader.read_controller()
        self.input_flip = control_pos.flip
        self.input_flip_up = control_pos.flip_up
        self.input_flip_down = control_pos.flip_down
        if control_pos.stop:
            self.last_stop_time = self.time_last
        # Check if we should halt the robot?
        halt = ((self.time_last - self.last_stop_time) < 1.5) or (not control_pos.signal)

        x = (control_pos.x / 127.0) 
        y = - (control_pos.y / 127.0) # y axis seems reversed.
        if halt:
            self.target_yaw = None # Do not spin when no signal.
            self.integral_error = 0
            x,y = 0,0
            self.input_flip = False
            self.input_flip_up = self.input_flip_down = False
            self.retract_time = 0 

        # Apply dead zone
        if abs(x ) < 0.2:
            x = 0
        if abs(y ) < 0.2:
            y = 0
        else:
            # Driving, enable retract
            self.retract_time = 0.4
        self.input_rotate = clamp(-1,1, x)
        self.input_drive = clamp(-1,1, y)
        if y == 0:
            last_nodrive_time = self.time_last



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
        self.time_delta = time_delta
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
        if (time_now - self.last_nodrive_time) < FAST_TIME:
            drive_speed *= DRIVE_SCALE_FAST
        else:
            drive_speed *= DRIVE_SCALE
        forward_speed = self.input_drive * drive_speed
        # Drive to rot + forward speed
        self.set_speeds(-rot + forward_speed, rot +forward_speed) 
        
        self.error_last = ang_error
        self.time_last = time_now
        self.tick_count += 1

    def process_flipper(self):
        # Process flipper state machine
        self.flip_timeout -= self.time_delta
        # If flip was pressed, and we are idle, do a flip.
        if self.input_flip and self.flip_state == 'idle':
            self.flip_state = 'flip'
            # Special retract
            if self.input_flip_down:
                self.flip_state = 'retract3'
            self.flip_timeout = FLIP_STATE_MAP[self.flip_state][2]
        # Move to next state
        if self.flip_timeout <=0:
            # Move to next state
            self.flip_state = FLIP_STATE_MAP[self.flip_state][3]
            self.flip_timeout = FLIP_STATE_MAP[self.flip_state][2]
        # set duty and direction
        direction = FLIP_STATE_MAP[self.flip_state][0]
        duty = FLIP_STATE_MAP[self.flip_state][1]
        # close flipper while driving
        if self.flip_state == 'idle' and self.retract_time > 0:
            direction, duty = -1,50
            self.retract_time -= self.time_delta
        # manual overrides for flip_up and down buttons (triangle and circle)
        if self.flip_state == 'idle':
            if self.input_flip_up:
                direction, duty = 1,40
            if self.input_flip_down:
                direction, duty = -1,40
        set_flipper(direction, duty)
            

def norm_angle(a):
    if a > 180:        
        return a - 360
    if a < -180:        
        return a + 360
    return a

def clamp(min_value, max_value, value):
    return min(max_value,max(min_value,value))

def wait_for_stillness():
    while True:
        i = read_last_imu()
        if i['motion']: 
            print("Still moving, waiting for stillness.")
        else:
            break

def main(): 
    try:
        init_socket()
        init_pigpio()
        set_flipper(0,0)
        cont = Controller()
        wait_for_stillness()
        # Do a little dance
        cont.set_speeds(0.2, 0.2)
        time.sleep(0.12)
        i = read_last_imu()
        if not i['motion']:
            raise Exception("Drive or IMU not working, cannot detect motion.")
        cont.set_speeds(0,0)
        time.sleep(1.0)
        cont.set_speeds(-0.2, -0.2)
        time.sleep(0.12)
        cont.set_speeds(0,0)
        time.sleep(1.0)
        # Done
        while True:
            cont.tick()
    finally:
        try:
            set_neutral()
            set_flipper(0,0)
        except Exception as e:
            pass # Ignore now

if __name__ == '__main__':
    main()
