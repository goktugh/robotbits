A7105-MR

This board is a copy (reverse engineerd) of the MD7105-SY module
made by unknown Chinese company. No schematic is publicly available
so I've tried to copy the datasheet and the layout in the modules.

The reason for reverse engineering the board is to create a compatibe copy
with a much smaller crystal, as the original is absolutely huge.

I'm not completely sure what the board thickness needs to be, it is a thin 
pcb, either 0.8mm or 1.0mm (need to check with some calipers maybe?)

I don't know exactly what values all the capacitors / resistor / inductors are.
Finding the values is difficult because they are 0402 smd packages and mounted on the board.

The a7105 datasheet contains a schematic which is mostly similar to the one here. Some parts
are missed out though (presumably they are not needed, the original modules 
work properly, after all)

Two of the inductors are made using PCB traces (microstrip inductors) and
their inductance is very approximate.

A svg reverse engineering the board is in ../a7105_module

Sorry to whoever's intellectual property is taken with this, but I don't expect they
get much royalty anyway on a module which sells at < $2


