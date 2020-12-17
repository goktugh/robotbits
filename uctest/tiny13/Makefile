
# We require the toolchain avr-gcc
# from ../../avr_toolchain
#
OBJDIR=obj

OBJ_FILES=main.o diag.o
OBJECTS=$(addprefix $(OBJDIR)/,$(OBJ_FILES))
HEADERS=diag.h

MCU=attiny13

ELF=$(OBJDIR)/main.elf
HEX=$(OBJDIR)/main.hex
BIN=$(OBJDIR)/main.bin

LINKFLAGS=-Os

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

CFLAGS=-mmcu=$(MCU)  -Os -Wall
$(OBJDIR)/%.o: %.c Makefile $(HEADERS)
	@mkdir -p $(OBJDIR)
	avr-gcc -c $(CFLAGS) -o $@ $<

# add -v here for heaps of debug
AVRDUDE_OPTS=-p t13 -c stk500v1 -P/dev/ttyUSB0 -b19200

# All hi fuses are 1 by default. 
# We want to enable the higher clock speed
# High fuse - the brownout detector,
# Low fuses - set the clock source to internal 9.6mhz
FUSES=-Uhfuse:w:0xfb:m -Ulfuse:w:0b00111010:m

install: link
	# Set fuses and program the image.
	avrdude $(AVRDUDE_OPTS) $(FUSES) -U flash:w:$(BIN):r 

reset:
	avrdude $(AVRDUDE_OPTS) 

clean:
	rm -r obj/
