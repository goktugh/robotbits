
# We require the avr-gcc toolchain
#
OBJDIR=obj_$(MCU)

OBJ_FILES=main.o diag.o motors.o rxin.o isense.o
OBJECTS=$(addprefix $(OBJDIR)/,$(OBJ_FILES))
HEADERS=diag.h motors.h rxin.h isense.h

MAKEFILES=Makefile common.mk

ELF=$(OBJDIR)/main.elf
HEX=$(OBJDIR)/main.hex

# LTOFLAGS=
LTOFLAGS=-flto
# -flto is link-time optimisation and must be used for compile and link.

LINKFLAGS=-Os $(LTOFLAGS)

link: $(OBJECTS) $(HEADERS)
	avr-gcc -mmcu=$(MCU) -o $(ELF) $(OBJECTS) $(LINKFLAGS)
	# see how big it is
	avr-size $(ELF)
	# disassemble, so can see what the compiler did
	avr-objdump -S $(ELF) > $(OBJDIR)/main.asm
	avr-objdump -t $(ELF) > $(OBJDIR)/main.sym
	avr-objdump -j .bss -j .data -t $(ELF) | grep 00 |sort > $(OBJDIR)/ram.txt # show ram usage
	avr-objcopy -j .text -j .data -j .rodata -O ihex $(ELF) $(HEX)
	avr-objcopy -j .text -j .data -j .rodata -O binary $(ELF) $(OBJDIR)/main.bin

CFLAGS += -mmcu=$(MCU) -Os -Wall  $(LTOFLAGS)
$(OBJDIR)/%.o: %.c $(MAKEFILES)
	@mkdir -p $(OBJDIR)
	avr-gcc -c $(CFLAGS) -o $@ $<

# Assembly language compile
$(OBJDIR)/%.o: %.S $(MAKEFILES) $(HEADERS)
	@mkdir -p $(OBJDIR)
	avr-gcc -c $(CFLAGS) -Xassembler -a=$(OBJDIR)/$(<).lst -o $@ $<

install: installpymcu
	
PYMCUPROG_OPTS=-d $(MCU_PYMCUPROG) -t uart -u /dev/ttyUSB0  
	
installpymcu: link
	pymcuprog $(PYMCUPROG_OPTS) erase
	pymcuprog $(PYMCUPROG_OPTS) -f $(HEX) -v info write 
	pymcuprog $(PYMCUPROG_OPTS) -f $(HEX) verify

clean:
	rm -rf $(OBJDIR)
