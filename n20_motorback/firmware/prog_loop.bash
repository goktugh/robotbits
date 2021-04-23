#!/bin/bash
set -x
# Optional parameter for MCU type on command-line
# example: t4, t5, t10 
# (Must be compatible with the firmware, obviously) 
MCU="${1:-t4}"
OPTS="-p $MCU -c usbasp -B 50khz"
while ((1)); do
    # Flash the image, and if that succeeds, also flash the fuse.
	avrdude $OPTS -U flash:w:obj/main.bin:r &&
        avrdude $OPTS -U fuse:w:0xfe:m
	sleep 4
done
