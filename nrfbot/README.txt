See https://gustavovelascoh.wordpress.com/2017/01/23/starting-development-with-nordic-nrf5x-and-gcc-on-linux/

1. Get arm toolchain (gcc) from

https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads

2. Get NRF SDK from

https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/nRF5_SDK_15.2.0_9412b96.zip

or v14 of the same.

3.  unpack all

4. modify the nrf SDK file Makefile.posix 
    to point GNU_INSTALL_ROOT to the right place

5. Examples BOARD_NAME should be BOARD_PCA10036 or  BOARD_PCA10040


To flash:

* Install Jlink from segger  https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPackBeta
    this is a .deb which installs in /opt/SEGGER/JLink
    this is where the nrfjprog expects to find it

* Install nrfjprog somehow

Useful flash commands are:

nrfjprog -f NRF52 --program _build/myblinky.hex --sectorerase
nrfjprog -f NRF52 --reset

