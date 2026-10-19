XB_ENV ?= /opt/toolchains/human68k-gcc11/bin
AS := $(XB_ENV)/human68k-gcc
CC := $(XB_ENV)/human68k-gcc
OBJCOPY := $(XB_ENV)/human68k-objcopy

# C build flags
CFLAGS := -std=c11 -O2 -Wall -Werror -I$(SRCDIR) -I$(XB) -I.
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
CFLAGS += -falign-functions=2 -flimit-function-alignment
CFLAGS += -fno-tree-loop-optimize
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
