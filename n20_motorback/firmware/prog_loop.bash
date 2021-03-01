#!/bin/bash
set -x
while ((1)); do
	avrdude -p t4 -c usbasp -U flash:w:obj/main.bin:r -B 50khz
	sleep 4
done
