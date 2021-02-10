FIRMWARE - for the Attiny4 (or 5, 9, 10)

"make install" - installs the image without fuse settings
"make verify"
"make writefuse" - will write the RSTDISBL fuse.
    This makes further programming tricky, because the reset pin is disabled.

"make resetfuse" - will restore the default fuses.    
# Reset the fuse bits - to do this we need to put +12v on the reset pin.


Tricks:
-------

We use -nostartfiles when linking, and provide our own init routine
in startup.c.

This saves flash by avoiding the vector table.

We need to be careful about using initialised global variables, they
won't work because I didn't include the code to copy the data from
flash into ram at boot.

