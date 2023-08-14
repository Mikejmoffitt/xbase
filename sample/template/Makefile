APPNAME := HELLO
OUTDIR := out
SRCDIR := src
RESDIR := res
OBJDIR := obj
XSP2LIBDIR := xsp2lib
XBASEDIR := xbase

# TODO: use GCC's automatic deps generation?
SOURCES_H := $(shell find ./$(SRCDIR)/ -type f -name '*.h') $(shell find ./$(XBASEDIR)/xbase/ -type f -name '*.h')

SOURCES_C := $(shell find ./$(SRCDIR)/ -type f -name '*.c') $(shell find ./$(XBASEDIR)/xbase/ -type f -name '*.c')
SOURCES_ASM := $(shell find ./$(SRCDIR)/ -type f -name '*.a68') $(shell find ./$(XBASEDIR)/xbase/ -type f -name '*.a68')

OBJECTS_C := $(addprefix $(OBJDIR)/, $(SOURCES_C:.c=.o))
OBJECTS_ASM := $(addprefix $(OBJDIR)/, $(SOURCES_ASM:.a68=.o))

# Physical target information.
TARGET_DEV := /dev/disk/by-id/usb-x68k_DEVDISK_000000000000-0:1

include $(XBASEDIR)/xb-rules.mk
# Can add anything to CFLAGS and ASFLAGS here with +=.
