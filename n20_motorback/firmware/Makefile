
# We require the toolchain avr-gcc
# from ../../avr_toolchain
#
OBJDIR=obj

OBJ_FILES=main.o startup.o
OBJECTS=$(addprefix $(OBJDIR)/,$(OBJ_FILES))
HEADERS=defs.h

MCU=attiny4

ELF=$(OBJDIR)/main.elf
HEX=$(OBJDIR)/main.hex
BIN=$(OBJDIR)/main.bin

# Flag(s) to enable link-time optimisation.
# LTO seems to reduce the binary size slightly.
LTO_FLAGS=-flto

# -nostartfiles 

LINKFLAGS=-Os $(LTO_FLAGS) -g -nostartfiles -nostdlib

link: $(OBJECTS) $(HEADERS)
	avr-gcc -mmcu=$(MCU) -o $(ELF) $(OBJECTS) $(LINKFLAGS)
	# see how big it is
	avr-size -A $(ELF)
	# disassemble, so can see what the compiler did
	avr-objdump -S $(ELF) > $(OBJDIR)/image.asm
	avr-objdump -t $(ELF) > $(OBJDIR)/image.sym
	avr-objdump -t $(ELF) |grep 00 |sort  > $(OBJDIR)/image-sorted.sym
	avr-objcopy -j .text -j .data -j .rodata -O ihex $(ELF) $(HEX)
	avr-objcopy -I ihex -O binary $(HEX) $(BIN)

CFLAGS=-mmcu=$(MCU)  -Os -Wall $(LTO_FLAGS) -g

$(OBJDIR)/%.o: %.c Makefile $(HEADERS)
	@mkdir -p $(OBJDIR)
	avr-gcc -c $(CFLAGS) -o $@ $<
	

AVRDUDE_OPTS=-p t4 -c usbasp

install:
	avrdude $(AVRDUDE_OPTS) -U flash:w:$(BIN):r

verify:
	avrdude $(AVRDUDE_OPTS) -U flash:v:$(BIN):r

# Bit 0 of the configuration is RSTDISBL, we need that to be 0
# All other bits 1 by default and we leave them.
writefuse:
	avrdude $(AVRDUDE_OPTS) -U fuse:w:0xfe:m

# Reset the fuse bits - to do this we need to put +12v on the reset pin.
# resetfuse:
# 	avrdude $(AVRDUDE_OPTS) -U fuse:w:0xff:m

reset:
	avrdude $(AVRDUDE_OPTS) 

clean:
	rm -r obj/
