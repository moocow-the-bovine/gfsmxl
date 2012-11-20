##-*- Mode: Makefile -*-

#OFLAGS  ?=-pipe -O2 -march=prescott -msse -msse2 -msse3 -mfpmath=sse -ffast-math
#OFLAGS  ?=-pipe -O2
#OFLAGS  ?=-pipe -g
OFLAGS   ?=-pipe -ggdb -O0
#OFLAGS  ?=-g -pg
#OFLAGS  ?=-O2 -pg

CC       = gcc
LD       = $(CC)

DEFS     ?=
WFLAGS   ?=-Wall -Winline
CFLAGS   =$(WFLAGS) $(DEFS) $(IFLAGS) $(OFLAGS)

#IFLAGS   ?=-I. $(shell pkg-config --cflags-only-I gfsm) -I./fib-1.1
IFLAGS   ?=-I. -I.. $(shell pkg-config --cflags-only-I gfsm)
LDFLAGS  =-L../.libs $(shell pkg-config --libs-only-L gfsm)  $(OFLAGS)
LIBS     =$(shell pkg-config --libs-only-l gfsm)

TARGETS = \
	gfsmxl-cascade-dlookup

KNOWN_TARGETS = \
	cascade.gfsc \
	gfsmxl-make-cascade \
	gfsmxl-cascade-get-arcs \
	gfsmxl-cascade-nbest \
	gfsmxl-cascade-nbest-many \
	gfsmxl-cascade-set-nth \
	gfsmxl-cascade-switch \
	gfsmxl-cascade-dlookup

##======================================================================
## RULES
##======================================================================
all: $(TARGETS)

known: $(KNOWN_TARGETS)

config:
	@echo "CC=$(CC)"
	@echo "LD=$(LD)"
	@echo "OFLAGS=$(OFLAGS)"
	@echo "DEFS=$(DEFS)"
	@echo "IFLAGS=$(IFLAGS)"
	@echo "WFLAGS=$(WFLAGS)"
	@echo "CFLAGS=$(CFLAGS)"
	@echo "LDFLAGS=$(LDFLAGS)"
	@echo "LIBS=$(LIBS)"

##======================================================================
## Automata & Indexing

%.gfsx: %.gfst
	gfsmindex $< -F $@

#	gfsmarcsort --mode=lwu $< | gfsmindex -F $@

cascade ?= abeditor.gfsx astar+bstar.gfsx

cascade: cascade.gfsc
cascade.gfsc: $(cascade) gfsmxl-make-cascade
	./gfsmxl-make-cascade $(cascade) > $@ || (rm -f $@; false)

##======================================================================
## Dependencies

LABINDEX_HDRS = ../gfsmxlLabelIndex.h ../gfsmxlLabelIndex.hi
LABINDEX_OBJ  = ../gfsmxlLabelIndex.o

CASCADE_HDRS  = $(LABINDEX_HDRS) ../gfsmxlCascade.h ../gfsmxlCascade.hi
CASCADE_OBJ   = $(LABINDEX_OBJ) ../gfsmxlCascade.o

HEAP_HDRS     = ../gfsmxlCLCFib.h ../gfsmxlCLCFibPriv.h
HEAP_OBJ      = ../gfsmxlCLCFib.o

LOOKUP_HDRS   = $(CASCADE_HDRS) $(HEAP_HDRS) ../gfsmxlCascadeLookup.h ../gfsmxlCascadeLookup.hi
LOOKUP_OBJ    = $(CASCADE_OBJ) $(HEAP_OBJ) ../gfsmxlCascadeLookup.o

GFSMXL_HDRS   = $(wildcard ../*.h) $(wildcard ../*.hi)
GFSMXL_OBJ    = ../.libs/libgfsmxl.a

gfsmxlCLCFib.o: ../gfsmxlCLCFib.c $(HEAP_HDRS)

gfsmxlLabelIndex.o: ../gfsmxlLabelIndex.c $(LABINDEX_HDRS)
gfsmxlCascade.o: ../gfsmxlCascade.c $(CASCADE_HDRS)
gfsmxlCascadeLookup.o: ../gfsmxlCascadeLookup.c $(LOOKUP_HDRS)

gfsmxl-make-cascade.o: gfsmxl-make-cascade.c $(CASCADE_HDRS)
gfsmxl-make-cascade: gfsmxl-make-cascade.o $(CASCADE_OBJ)

gfsmxl-cascade-get-arcs.o: gfsmxl-cascade-get-arcs.c $(CASCADE_HDRS)
gfsmxl-cascade-get-arcs: gfsmxl-cascade-get-arcs.o $(CASCADE_OBJ)

gfsmxl-cascade-nbest.o: gfsmxl-cascade-nbest.c $(LOOKUP_HDRS)
gfsmxl-cascade-nbest: gfsmxl-cascade-nbest.o $(LOOKUP_OBJ)

gfsmxl-cascade-nbest-many.o: gfsmxl-cascade-nbest-many.c $(LOOKUP_HDRS)
gfsmxl-cascade-nbest-many: gfsmxl-cascade-nbest-many.o $(LOOKUP_OBJ)

gfsmxl-cascade-set-nth.o: gfsmxl-cascade-set-nth.c $(GFSMXL_HDRS)
gfsmxl-cascade-set-nth: gfsmxl-cascade-set-nth.o $(GFSMXL_OBJ)

gfsmxl-cascade-switch.o: gfsmxl-cascade-switch.c $(GFSMXL_HDRS)
gfsmxl-cascade-switch: gfsmxl-cascade-switch.o $(GFSMXL_OBJ)

gfsmxl-cascade-dlookup.o: gfsmxl-cascade-dlookup.c $(GFSMXL_HDRS)
gfsmxl-cascade-dlookup: gfsmxl-cascade-dlookup.o $(GFSMXL_OBJ)

##======================================================================
## Rules: linking: specialized

##======================================================================
## Rules: generic: linking
gfsmxl%: gfsmxl%.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

gfsmxl-%: gfsmxl-%.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

##======================================================================
## Rules: generic: C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

##======================================================================
## Rules: cleanup
clean:
	rm -f $(KNOWN_TARGETS) *.o *.a *.so fib-1.1/*.o

realclean: clean
	rm -f ab*.gfsx ab*.gfst astar*.gfst astar*.gfsx *.gfsc

##======================================================================
## Inherited
include fsmhacks.mak
