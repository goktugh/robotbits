#!/bin/bash

set -x
java -jar /home/mark/unpack/kicad-util/target/kicadutil-1.0-SNAPSHOT.jar \
    pcb -f panel4.kicad_pcb panel --inset=-0.1 --width=4.0 --pitch 0.8 --hole 0.6
