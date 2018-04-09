#!/usr/bin/env python3
import json

def estimate_pulse(caldata, channel, power):
    # power is from -1 to 1.
    # channel is either "left" or "right"

    # Filter the channel we want
    pulse_and_speed = []
    for cal_channel, cal_pulse, cal_speed in caldata:
        if cal_channel == channel:
            pulse_and_speed.append( (cal_pulse, cal_speed) )
    pulse_and_speed = list(reversed(sorted(pulse_and_speed)))

    # print(repr(pulse_and_speed))
    # Find min / max
    speeds = list(map(lambda t: t[1], pulse_and_speed))
    minspeed = min(speeds)
    maxspeed = max(speeds)
            
    # Estimate desired speed.
    if power == 0:
        desired_speed = 0 
    if power > 0:
        desired_speed = power * maxspeed
    if power < 0:
        desired_speed = - (power * minspeed)
    #print("min={}, max={} desired={}".format(minspeed,maxspeed, desired_speed))
    # Find the lowest sample which is >= to the target speed
    i = 0
    while i < len(pulse_and_speed) and (pulse_and_speed[i][1] < desired_speed):
        i += 1
    if i == 0:
        i = 1 # Avoid edge.
    # Interpolate between i-1 and i
    #print("i={}".format(i))
    #print("at i-1: " + repr(pulse_and_speed[i-1]))
    #print("at i: " + repr(pulse_and_speed[i]))
    return linear_interpolate(
        pulse_and_speed[i-1][0],
        pulse_and_speed[i-1][1],
        pulse_and_speed[i][0],
        pulse_and_speed[i][1],
        desired_speed)
    
def linear_interpolate(pulse0, val0, pulse1, val1, target):
    gradient = (val1 - val0) / (pulse1 - pulse0)
    diff0 = target - val0
    return pulse0 + (diff0 * gradient)

def fixup_cal_data(caldata):
    # first do some rounding.
    for i in range(len(caldata)):
        caldata[i][2] = round(caldata[i][2])
    # Make sure that the calibration data always goes from
    # slower to faster speeds.
    print(repr(list(map(lambda t: t[2], caldata))))
    for channel in ['left']:
        maxspeed = -1000
        for i in reversed(range(len(caldata))):
            cal_channel = caldata[i][0]
            cal_speed = caldata[i][2]
            if cal_channel == channel:
                if cal_speed < maxspeed:
                    # Speed decreased?
                    # Set it to the max seen so far.
                    caldata[i][2] = maxspeed
                maxspeed = max(cal_speed, maxspeed)
    print(repr(list(map(lambda t: t[2], caldata))))

def main():
    with open('data/calibrate0.json', 'rt') as f:
        caldata = json.load(f)
    fixup_cal_data(caldata); raise Exception("FIXME")
    # Need to find the min/max speeds
    channel = 'left'
    for powerint in range(-15,16):
        print("power: {} of 15".format(powerint))
        power = powerint / 15.0
        print("Pulse: {}".format(estimate_pulse(caldata, channel, power)))

if __name__ == '__main__':
    main()

