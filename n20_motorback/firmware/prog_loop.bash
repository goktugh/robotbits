#!/bin/bash
set -x
OPTS="-p t4 -c usbasp -B 50khz"
while ((1)); do
    # Flash the image, and if that succeeds, also flash the fuse.
	avrdude $OPTS -U flash:w:obj/main.bin:r &&
        avrdude $OPTS -U fuse:w:0xfe:m
	sleep 4
done
