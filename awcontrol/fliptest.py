#!/usr/bin/env python3
import RPi.GPIO as GPIO

import time

PIN_ENABLE = 16
PIN_RUN0 = 20
PIN_RUN1 = 21

GPIO.setmode(GPIO.BCM)


GPIO.setup(PIN_ENABLE, GPIO.OUT, initial=0)
GPIO.setup(PIN_RUN0, GPIO.OUT, initial=0)
GPIO.setup(PIN_RUN1, GPIO.OUT, initial=0)

# Now pulse them

GPIO.output(PIN_RUN0, 1)
GPIO.output(PIN_RUN1, 0)

# Do the flip....
GPIO.output(PIN_ENABLE, 1)
time.sleep(0.2)
# Brake on
GPIO.output(PIN_RUN0, 0)
time.sleep(0.2)
# All off
GPIO.output(PIN_ENABLE, 0)

print("Done")
GPIO.output(PIN_RUN0, 0)
GPIO.output(PIN_RUN1, 0)

GPIO.cleanup()
