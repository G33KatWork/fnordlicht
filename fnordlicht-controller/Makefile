####################################################
# fnordlicht-ng Makefile
####################################################

# ATTENTION:
# Any of these variables are overridden with the values from the file
# "config.mk", you can tune your settings there.  The file "config.mk" is not
# under version control.
# Just run 'make' to create a "config.mk" with the default settings

# controller
MCU = atmega168

# frequency
F_CPU = 16000000UL

# main application name (without .hex)
# eg 'test' when the main function is defined in 'test.c'
TARGET = fcontrol

# c sourcecode files
# eg. 'test.c foo.c foobar/baz.c'
SRC = $(wildcard *.c) usbdrv/usbdrv.c

# asm sourcecode files
# eg. 'interrupts.S foobar/another.S'
ASRC = $(wildcard *.S) usbdrv/usbdrvasm.S

# headers which should be considered when recompiling
# eg. 'global.h foobar/important.h'
HEADERS = $(wildcard *.h)

# include directories (used for both, c and asm)
# eg '. usbdrv/'
INCLUDES = . usbdrv/


# use more debug-flags when compiling
DEBUG = 0

# default baudrate
CONFIG_SERIAL_BAUDRATE = 19200

# avrdude programmer protocol
PROG = usbasp
# avrdude programmer device
DEV = usb
# further flags for avrdude
AVRDUDE_FLAGS =

CFLAGS += -DCONFIG_SERIAL_BAUDRATE=$(CONFIG_SERIAL_BAUDRATE)

# use a custom linker script
LDFLAGS += -L$(CURDIR)/ldscripts

.PHONY: all

# main make target (moved up here because of the config.mk target)
all: $(TARGET).hex

# create config.mk (if it does not exist yet)
$(CURDIR)/config.mk:
	@$(CP) config.mk.template config.mk
	@echo "===================================================="
	@echo "created file $@"
	@echo "please tune your settings there, then run 'make' again"
	@echo "===================================================="
	@exit 1

# include config file
-include $(CURDIR)/config.mk

####################################################
# 'make' configuration
####################################################
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AS = avr-as
SIZE = avr-size
CP = cp
RM = rm -f
RMDIR = rm -rf
MKDIR = mkdir
AVRDUDE = avrdude

# flags for automatic dependency handling
DEPFLAGS = -MD -MP -MF .dep/$(@F).d

# flags for the compiler (for .c files)
CFLAGS += -g -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -std=gnu99 -fshort-enums $(DEPFLAGS)
CFLAGS += $(addprefix -I,$(INCLUDES))
CFLAGS += -fno-split-wide-types
CFLAGS += --param inline-call-cost=2  -finline-limit=3 -fno-inline-small-functions
#CFLAGS += -Wl,--relax

# flags for the compiler (for .S files)
ASFLAGS += -g -mmcu=$(MCU) -DF_CPU=$(F_CPU) -x assembler-with-cpp $(DEPFLAGS)
ASFLAGS += $(addprefix -I,$(INCLUDES))

# flags for the linker
LDFLAGS += -mmcu=$(MCU)

# fill in object files
OBJECTS += $(SRC:.c=.o)
OBJECTS += $(ASRC:.S=.o)

# include more debug flags, if $(DEBUG) is 1
ifeq ($(DEBUG),1)
	CFLAGS += -Wall -W -Wchar-subscripts -Wmissing-prototypes
	CFLAGS += -Wmissing-declarations -Wredundant-decls
	CFLAGS += -Wstrict-prototypes -Wshadow -Wbad-function-cast
	CFLAGS += -Winline -Wpointer-arith -Wsign-compare
	CFLAGS += -Wunreachable-code -Wdisabled-optimization
	CFLAGS += -Wcast-align -Wwrite-strings -Wnested-externs -Wundef
	CFLAGS += -Wa,-adhlns=$(basename $@).lst
	CFLAGS += -DCONFIG_DEBUG=1
endif

####################################################
# avrdude configuration
####################################################
ifeq ($(MCU),atmega8)
	AVRDUDE_MCU=m8
endif
ifeq ($(MCU),atmega48)
	AVRDUDE_MCU=m48
endif
ifeq ($(MCU),atmega88)
	AVRDUDE_MCU=m88
endif
ifeq ($(MCU),atmega168)
	AVRDUDE_MCU=m168
endif

AVRDUDE_FLAGS += -p $(AVRDUDE_MCU)

####################################################
# make targets
####################################################

.PHONY: clean distclean avrdude-terminal

$(TARGET).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS)

# all objects (.o files) and config.mk
$(OBJECTS): $(HEADERS) config.mk

# remove all compiled files
clean:
	$(RM) $(foreach ext,elf hex eep.hex map,$(TARGET).$(ext)) \
		$(foreach file,$(patsubst %.o,%,$(OBJECTS)),$(foreach ext,o lst lss,$(file).$(ext)))

# additionally remove the dependency makefile and config.mk
distclean: clean
	$(RMDIR) .dep
	$(RM) config.mk

# avrdude-related targets
install program: program-$(TARGET)

avrdude-terminal:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -c $(PROG) -P $(DEV) -t

program-%: %.hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -c $(PROG) -P $(DEV) -U flash:w:$<

program-eeprom-%: %.eep.hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -c $(PROG) -P $(DEV) -U eeprom:w:$<

# special programming targets
%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	@echo "===================================================="
	@echo "$@ compiled for fnordlicht-controller"
	@echo "using controller $(MCU)"
	@echo -n "size for $< is "
	@$(SIZE) -A $@ | grep '\.sec1' | tr -s ' ' | cut -d" " -f2
	@echo "===================================================="

%.eep.hex: %.elf
	$(OBJCOPY) --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex -j .eeprom $< $@

%.lss: %.elf
	$(OBJDUMP) -h -S $< > $@

.PHONY: fuses-atmega8-fnordlichtmini-with-bootloader fuses-lock

fuses-atmega8-fnordlichtmini-with-bootloader:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -c $(PROG) -P $(DEV) -U lfuse:w:0x3f:m -U hfuse:w:0xda:m

fuses-atmega8-fnordlichtmini-without-bootloader:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -c $(PROG) -P $(DEV) -U lfuse:w:0x3f:m -U hfuse:w:0xd9:m

fuses-lock:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -c $(PROG) -P $(DEV) -U lock:w:0x2f:m

-include $(shell $(MKDIR) .dep 2>/dev/null) $(wildcard .dep/*)
