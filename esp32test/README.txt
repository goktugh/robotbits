HOW IT WORKS
------------

Radio / wifi:

- Receive UDP packets from the controller, with the commands:

MODE: STOP (everything off)
DRIVE (tank-mixing maybe, drive L and R motors like a tank at
    very low speed) - throttle ignored.
SPIN ( Melty!)

Throttle = for melty (no effect in tank-mode) - control *spin speed*
Rotate virtual front - L / R (degrees per second) - on left/right stick
Virtual movement = fwd / back (maybe left or right too, why not?!)

Trim rotation speed = this is an analogue input which tweaks the cycle time
    Can put that on a twisty knob to ajdust the cycle time.



HARDWARE USAGE:
-----

Motor outputs (PWM or DSHOT)
GPIO25
GPIO26

Motor Feedback:
GPIO9 / U1RXD = This pin is used for uart telemetry from ESCs

Sensor
------
Accelerometer  MMA1200KEG
GPIO4 = ACCEL_SENSE (Analogue)
GPIO5 = ACCEL_STATUS (Digital input)
GPIO33 = ACCEL_SELF_TEST (Digital output)

Voltage is divided by 2.

So from the MCU, zeroG = approx 1.25V, approximately 4mv per G

Maximum range is approximately 250G which would give approx 2.25V
But all that will need to be calibrated.

Also the ESP32 ADC is maybe not very good.

Programming / debug
-------------------
soicbite connector which breaks out these signals
1. VLOGIC (3.3v)
2. GPIO0 (PROG_BOOT) - low to boot from serial
3. GPIO2
4. GND
5. NC
6. U0TXD
7. U0RXD
8. EN (RESET) - active low, pulled high



GPIO0 (boot mode flag)
GPIO2 

----
TODO:

1. Get the brushless ESC working with 1104 motor
    + Malenki-Nano or something

1. Complete the home-made bench power supply.
    
2. Get the esp32 generating servo pulses
    - wire to brushless esc
    - Get brushless going under control of esp32
    
3. Get the esp generating dshot command
    3a. Send throttle level via dshot (48...2047)
    3b. Enable 3d mode and save
    3c. Test reversing via dshot

4. Build wheels with silicone tyres - to fit snugly
    over motor outrunner
    Or just tyres directly over outrunner?
