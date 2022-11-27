APPNAME := png2xsp

CC := gcc
INSTALL_PREFIX := /usr/bin
ifdef SYSTEMROOT
	APPEXT := .exe
endif

SOURCES := $(APPNAME).c lodepng.c

EXECNAME := $(APPNAME)$(APPEXT)

.PHONY: clean

all: $(EXECNAME)

$(EXECNAME): $(SOURCES)
	$(CC) $^ -O3 -Wall -o $@

install: $(EXECNAME)
	cp $< $(INSTALL_PREFIX)/

clean:
	rm -f $(EXECNAME)
