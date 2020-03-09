This is an experiment, to see if we can use the CCL units
to output PWM form TCD, because we are running out of timers on
the device.


PROBLEM:

TCD cannot output on any of the pins that we have available and most pins cannot
be changed.

SOLUTION:

CCL = configurable custom logic

we want to use the CCL to simply copy the output of the TCD comparators
to the appropriate pins

LUT1-OUT on PA7 (pin 8 on the 3217)

- Use the LUT1 with an input from the TCD timeout?

LUT0-OUT on PB4 (pin 12)
- "Alternate" pin

HOW DO WE DO IT?

CCL input selection

INSEL0 - can take input from TCD WOA
INSEL1 - can take input from TCD WOB
INSEL2 - don't care, set it to masked

- Mask the other inputs (set them to a fixed value, probably aways 0)
- enabe the output
- Set up truth table so the CCL logically "OR" 
    any of its inputs, 
    
TCD SETUP
--------

CLOCK SETUP
---

Set to 10mhz

PINOUT
------

3=GND
4=Power

14=PB2 / txd - debug serial output

5=PA4 / TCD WOA - output from the WOA channel in TCD
6=PA5 / TCD WOB - output from the WOB channel in TCD

12=PB4 / LUT0-OUT
18=PC1 / LUT1-OUT
