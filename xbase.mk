#
# XBase rules and sources.
#

XB_SOURCES_ASM := \
	$(XB)/xbase/pal.a68 \
	$(XB)/xbase/joy.a68 \
	$(XB)/xbase/ipl.a68 \
	$(XB)/xbase/opm_commit.a68 \
	$(XB)/xbase/util/fade.a68 \
	$(XB)/xbase/util/scroll.a68 \
	$(XB)/xbase/util/vbl_wait.a68 \
#

XB_SOURCES_C := \
	$(XB)/xbase/crtc.c \
	$(XB)/xbase/pcg.c \
	$(XB)/xbase/mfp.c \
	$(XB)/xbase/keys.c \
#

XB_OBJECTS_C   := $(patsubst $(XB)/%.c,$(XBOBJDIR)/%.o,$(XB_SOURCES_C))
XB_OBJECTS_ASM := $(patsubst $(XB)/%.a68,$(XBOBJDIR)/%.o,$(XB_SOURCES_ASM))

include $(XB)/mk/m68k-flags.mk
