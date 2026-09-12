# Assumes directories have already been set.
# Typical defaults:
#
# OUTDIR := out
# APPNAME := HELLO
# SRCDIR := src
# RESDIR := res
# OBJDIR := obj
# XBASEDIR := xbase

# GCC binaries

HUMAN68K := /opt/toolchains/human68k

AS := $(HUMAN68K)/bin/m68k-elf-gcc
CC := $(HUMAN68K)/bin/m68k-elf-gcc
# OBJCOPY := $(HUMAN68K)/bin/m68k-elf-objcopy
ELF2X := $(HUMAN68k)/bin/elf2x.py

# C build flags
CFLAGS := -std=gnu23 -O2 -Wall -Werror -I$(SRCDIR) -I$(XBASEDIR) -I. -I$(HUMAN68K)/include
CFLAGS += -fcall-used-d2 -fcall-used-a2  # Compatibility with XSP.
CFLAGS += -mcpu=68000
CFLAGS += -fomit-frame-pointer
CFLAGS += -frename-registers
CFLAGS += -fshort-enums
CFLAGS += -fconserve-stack
CFLAGS += -fwrapv
CFLAGS += -fno-gcse
CFLAGS += -fms-extensions
CFLAGS += -fno-web -fno-unit-at-a-time
# TODO: Define X68000 memory map as extern uint8_t arrays and have linker
# properly point them to the correct locations.
# TODO: array-bounds and stringop-overflow are bugged on GCC 11.2.0
# CFLAGS += -Wno-array-bounds -Wno-stringop-overflow

# Assembler flags
ASFLAGS := $(CFLAGS)
ASFLAGS += -I$(SRCDIR) -I$(OBJDIR) -I$(XBASEDIR) -I$(HUMAN68K)/include
ASFLAGS += -Wa,-I$(SRCDIR) -Wa,-I$(OBJDIR) -Wa,-I$(XBASEDIR)
ASFLAGS += -mcpu=68000 -Wa,-m68000
ASFLAGS += -Wa,--register-prefix-optional
ASFLAGS += -Wa,--bitwise-or
ASFLAGS += -x assembler-with-cpp

# Linker flags
LDFLAGS := -L$(HUMAN68K)/lib
LDFLAGS += -nostartfiles
LDFLAGS += -Wl,-q
LDFLAGS += -Wl,--no-warn-rwx-segments
LDFLAGS += -Wl,-Map=$(APPNAME).map
LDFLAGS += -T $(HUMAN68K)/ld_scripts/human68kxfile.x
LDFLAGS += --specs=picolibc.specs
LDFLAGS += -lgcc -lc -los

#
# Build Rules
#

.PHONY: all clean xb_copy_resources $(OUTDIR)/$(APPNAME).x $(EXTERNAL_DEPS)

# Generic variable for additional files that are a build prerequisite.
EXTERNAL_DEPS ?=
EXTERNAL_ARTIFACTS ?=

all: $(OUTDIR)/$(APPNAME).x

xb_copy_resources:
	@mkdir -p $(OUTDIR)
	@cp -r $(RESDIR)/* $(OUTDIR)/

$(OUTDIR)/$(APPNAME).x: $(OBJECTS_C) $(OBJECTS_ASM) xb_copy_resources
	@bash -c 'printf "\t\e[94m[ LNK ]\e[0m $(OBJECTS_ASM) $(OBJECTS_C)\n"'
	$(CC) -o $(APPNAME).elf $(LDFLAGS) $(CFLAGS) $(OBJECTS_C) $(OBJECTS_ASM)
	@mkdir -p $(OUTDIR)/
	python3 $(ELF2X) $(APPNAME).elf $@
# 	$(OBJCOPY) -v -O xfile $(APPNAME).bin $(OUTDIR)/$(APPNAME).x > /dev/null
#	@rm $(APPNAME).elf
	@bash -c 'printf "\e[92m\n\tBuild Complete. \e[0m\n\n"'

$(OBJDIR)/%.o: %.c $(XSP2LIBDIR) $(SOURCES_H) $(EXTERNAL_DEPS)
	@mkdir -p $(OBJDIR)/$(<D)
	@bash -c 'printf "\t\e[96m[  C  ]\e[0m $<\n"'
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/%.o: %.a68 $(XSP2LIBDIR) $(SOURCES_H) $(EXTERNAL_DEPS)
	@mkdir -p $(OBJDIR)/$(<D)
	@bash -c 'printf "\t\e[95m[ ASM ]\e[0m $<\n"'
	gawk '{gsub(/;/,";#"); printf("%s", $$0 RT)}' RS='"[^"]*"' $< | gawk '{gsub(/\$$/,"0x"); printf("%s", $$0 RT)}' RS='"[^"]*"' | $(AS) $(ASFLAGS) -o $@ -c -

clean:
	$(RM) $(OBJECTS_C) $(OBJECTS_ASM)
	rm -rf $(APPNAME).map
	rm -rf $(OBJDIR)
	rm -rf $(OUTDIR)
	rm -rf $(XSP2LIBDIR)
	echo $(EXTERNAL_ARTIFACTS) | xargs --no-run-if-empty rm -rf $(EXTERNAL_ARTIFACTS)

#
# Copying to USB-mounted filesystem
#
# On the X68000 side, run SUSIE.X <Drive>: -ID<SCSI ID>
# To find the SCSI ID, just run SUSIE.X without arguments.
#
upload: $(OUTDIR)/$(APPNAME).x
	mkdir -p /tmp/mo_mnt
	-sudo umount $(TARGET_DEV)
	sudo mount $(TARGET_DEV) /tmp/mo_mnt
	sudo rm -rf /tmp/mo_mnt/*
	mkdir -p uccopy
	cp -r $(OUTDIR)/* uccopy/
	find uccopy/* -type f,d -iname "*" -execdir rename "y/a-z/A-Z/" {} +
	sudo cp -r uccopy/* /tmp/mo_mnt/
	sync
	rm -rf uccopy
	sudo umount $(TARGET_DEV)
	rmdir /tmp/mo_mnt
