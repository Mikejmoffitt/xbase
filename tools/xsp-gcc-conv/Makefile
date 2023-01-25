# Very janky means of building XSPlib with GCC.

LIBNAME := xsp2lib

# Generated output from the converter
GENDIR := gen
# Original XSP HAS source.
HASSRCDIR := x68k_xsp/XSP
# Where the converted sources go.
CONVSRCDIR := $(GENDIR)/src
# Outputs
OUTDIR := out

# Utilities and configuration.
RM := rm
ICONV := iconv
AS := human68k-as
AR := human68k-ar
MKDIR := mkdir
SRCCNV := python3 xsp_syntax_conv.py
GIT := git

ASFLAGS += -m68000 -R --base-size-default-16 -l --bitwise-or --register-prefix-optional -I. -I$(CONVSRCDIR)

TOPLEVEL_SRC := $(CONVSRCDIR)/XSPsys.s cfuncs.s

all: $(OUTDIR)/$(LIBNAME).a

.PHONY: $(CONVSRCDIR)

$(HASSRCDIR):
	$(GIT) clone git@github.com:yosshin4004/x68k_xsp

$(CONVSRCDIR): $(HASSRCDIR)
	$(MKDIR) -p $(GENDIR)
	$(MKDIR) -p $@
	$(ICONV) -f sjis -t utf8 $(HASSRCDIR)/XSPsys.s > temp.s && $(SRCCNV) temp.s $@/XSPsys.s
	$(ICONV) -f sjis -t utf8 $(HASSRCDIR)/XSPfnc.s > temp.s && $(SRCCNV) temp.s $@/XSPfnc.s
	$(ICONV) -f sjis -t utf8 $(HASSRCDIR)/XSPout.s > temp.s && $(SRCCNV) temp.s $@/XSPout.s
	$(ICONV) -f sjis -t utf8 $(HASSRCDIR)/XSPset.s > temp.s && $(SRCCNV) temp.s $@/XSPset.s
	$(ICONV) -f sjis -t utf8 $(HASSRCDIR)/XSPsetas.s > temp.s && $(SRCCNV) temp.s $@/XSPsetas.s
	$(ICONV) -f sjis -t utf8 $(HASSRCDIR)/XSP128.s > temp.s && $(SRCCNV) temp.s $@/XSP128.s
	$(ICONV) -f sjis -t utf8 $(HASSRCDIR)/XSP512.s > temp.s && $(SRCCNV) temp.s $@/XSP512.s
	$(ICONV) -f sjis -t utf8 $(HASSRCDIR)/XSP512b.s > temp.s && $(SRCCNV) temp.s $@/XSP512b.s
	$(ICONV) -f sjis -t utf8 $(HASSRCDIR)/XSPmem.s > temp.s && $(SRCCNV) temp.s $@/XSPmem.s
	$(RM) temp.s

$(OUTDIR)/$(LIBNAME).o: $(CONVSRCDIR) $(OUTDIR) $(TOPLEVEL_SRC) 
	$(AS) $(ASFLAGS) -c $(TOPLEVEL_SRC) -o $@

$(OUTDIR)/$(LIBNAME).a: $(OUTDIR)/$(LIBNAME).o
	$(AR) cr $@ $<

$(OUTDIR):
	$(MKDIR) -p $@

clean: $(OUTDIR)
	$(RM) -rf $(OUTDIR)
	$(RM) -rf $(GENDIR)
