# Assumes directories have already been set.
# Typical defaults:
#
# OUTDIR := out
# APPNAME := HELLO
# SRCDIR := src
# RESDIR := res
# OBJDIR := obj
# XSP2LIBDIR := xsp2lib
# XBASEDIR := xbase

# GCC binaries
AS := human68k-gcc
CC := human68k-gcc
OBJCOPY := human68k-objcopy

# C build flags
CFLAGS := -std=c11 -O2 -Wall -Werror -I$(SRCDIR) -I$(XBASEDIR) -I.
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
CFLAGS += -fno-strict-aliasing
# TODO: Define X68000 memory map as extern uint8_t arrays and have linker
# properly point them to the correct locations.
# TODO: array-bounds and stringop-overflow are bugged on GCC 11.2.0
CFLAGS += -Wno-array-bounds -Wno-stringop-overflow

# Assembler flags
ASFLAGS := $(CFLAGS)
ASFLAGS += -I$(SRCDIR) -I$(OBJDIR) -I$(XBASEDIR)
ASFLAGS += -Wa,-I$(SRCDIR) -Wa,-I$(OBJDIR) -Wa,-I$(XBASEDIR)
ASFLAGS += -mcpu=68000 -Wa,-m68000
ASFLAGS += -Wa,--register-prefix-optional
ASFLAGS += -Wa,--bitwise-or
ASFLAGS += -x assembler-with-cpp

# Linker flags
LDFLAGS := -Wl,-q
LDFLAGS += -Wl,-Map=$(APPNAME).map

#
# Build Rules
#

.PHONY: all clean xb_copy_resources $(OUTDIR)/$(APPNAME).X $(XSP2LIBDIR) $(EXTERNAL_DEPS)

EXTERNAL_DEPS ?=
EXTERNAL_ARTIFACTS ?=
TMP_XBDATA := /tmp/xbdata

all: $(OUTDIR)/$(APPNAME).X

# Create copy of resources directory with filtered filenames
xb_copy_resources: $(EXTERNAL_DEPS)
	rm -rf $(TMP_XBDATA)/
	mkdir -p $(TMP_XBDATA)/
	cp -r $(RESDIR)/* $(TMP_XBDATA)/
	find $(TMP_XBDATA)/*/ -type f -execdir rename -f "y/a-z/A-Z/" {} +
	find $(TMP_XBDATA)/*/ -type d -execdir rename -f "y/a-z/A-Z/" {} +

$(OUTDIR)/$(APPNAME).X: $(OBJECTS_C) $(OBJECTS_ASM) $(EXTERNAL_DEPS) xb_copy_resources
	@bash -c 'printf "\t\e[94m[ LNK ]\e[0m $(OBJECTS_ASM) $(OBJECTS_C)\n"'
	$(CC) -o $(APPNAME).bin $(LDFLAGS) $(CFLAGS) $(OBJECTS_C) $(OBJECTS_ASM) $(XSP2LIBDIR)/xsp2lib.a
	mkdir -p $(OUTDIR)
	$(OBJCOPY) -v -O xfile $(APPNAME).bin $(OUTDIR)/$(APPNAME).X > /dev/null
	rm $(APPNAME).bin
	cp -r $(TMP_XBDATA)/* $(OUTDIR)/
	@bash -c 'printf "\e[92m\n\tBuild Complete. \e[0m\n\n"'

$(OBJDIR)/%.o: %.c $(XSP2LIBDIR) $(SOURCES_H) $(EXTERNAL_DEPS)
	mkdir -p $(OBJDIR)/$(<D)
	bash -c 'printf "\t\e[96m[  C  ]\e[0m $<\n"'
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/%.o: %.a68 $(XSP2LIBDIR) $(SOURCES_H) $(EXTERNAL_DEPS)
	mkdir -p $(OBJDIR)/$(<D)
	bash -c 'printf "\t\e[95m[ ASM ]\e[0m $<\n"'
	gawk '{gsub(/;/,";#"); printf("%s", $$0 RT)}' RS='"[^"]*"' $< | gawk '{gsub(/\$$/,"0x"); printf("%s", $$0 RT)}' RS='"[^"]*"' | $(AS) $(ASFLAGS) -o $@ -c -

clean:
	$(RM) $(OBJECTS_C) $(OBJECTS_ASM)
	rm -rf $(APPNAME).map
	rm -rf $(OBJDIR)
	rm -rf $(OUTDIR)
	rm -rf $(XSP2LIBDIR)
	echo $(EXTERNAL_ARTIFACTS) | xargs --no-run-if-empty rm -rf $(EXTERNAL_ARTIFACTS)

#
# Building XSP for GCC
#
$(XSP2LIBDIR): $(XBASEDIR)/tools/xsp-gcc-conv/
	mkdir -p $@
	cd $< && make
	cp $</out/* $@

#
# Copying to USB-mounted filesystem
#
# On the X68000 side, run SUSIE.X <Drive>: -ID<SCSI ID>
# To find the SCSI ID, just run SUSIE.X without arguments.
#
upload: $(OUTDIR)/$(APPNAME).X
	mkdir -p /tmp/mo_mnt
	-sudo umount $(TARGET_DEV)
	sudo mount $(TARGET_DEV) /tmp/mo_mnt
	sudo rm -rf /tmp/mo_mnt/*
	sudo cp -r $(OUTDIR)/* /tmp/mo_mnt/
	sync
	sudo umount $(TARGET_DEV)
	rmdir /tmp/mo_mnt
