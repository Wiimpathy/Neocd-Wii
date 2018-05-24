##########################
# Makefile for Z80 Core  #
##########################

BUILDDIR=.

#setup C Flags
CFLAGS= -I. -I./$(BUILDDIR) -O3 -fomit-frame-pointer -fno-ident -funsigned-char -DINLINE="static inline" -DCLEANBUILD=1 \
	-Wno-sign-compare -Wpointer-arith -Wcast-align -Wshadow -fstrict-aliasing -D__ASMPREFIX__

all: $(BUILDDIR)/libz80.a

clean:
	rm -f $(BUILDDIR)/libz80.a

$(BUILDDIR)/libz80.a: $(BUILDDIR)/z80.o $(BUILDDIR)/z80daisy.o
	@$(AR) -r $@ $^

$(BUILDDIR)/z80.o: z80.c z80.h
	@echo ""
	@echo "*** Mame Z80 Core **********************************************************"
	@echo "    Copyright (C) 1998,1999,2000 Juergen Buchmueller, all rights reserved."
	@echo "****************************************************************************"
	@echo ""
	@$(CC) $(CFLAGS) -c z80.c -o $(BUILDDIR)/z80.o

$(BUILDDIR)/z80daisy.o: z80daisy.c z80daisy.h
	@$(CC) $(CFLAGS) -c z80daisy.c -o $(BUILDDIR)/z80daisy.o
