# Raspberry Pi Pico
# PWM accuracy tester. Pico has a crystal so should be really accurate.


import machine
from machine import Pin

pin = machine.Pin(19, machine.Pin.OUT)
pwm = machine.PWM(pin)
pwm.freq(50)

def set_pulse(width):
    # duty will be 65536 = 100% or 0 = 0%
    # 100% = 20 milliseconds
    # so 10% = 2000 millseconds
    duty = (width * 6554) // 2000
    print("Setting duty to {}".format(duty))
    pwm.duty_u16(duty)
    
while True:
    width = int(input("Pulse width? "))
    set_pulse(width)
    
# 1400 = min rev
# 1720 = min fwd
# Centre pos approx 1560


    
