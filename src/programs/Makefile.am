## File: src/programs/Makefile.am
## Package: *
## Description:
##   + program-source-level automake file
##
## Process this file with Automake to create Makefile.in.
##-----------------------------------------------------------------------

##-----------------------------------------------------------------------
## subdirectories
##-----------------------------------------------------------------------
#SUBDIRS =

##-----------------------------------------------------------------------
## primary targets
##-----------------------------------------------------------------------

## --- executable programs
#bin_PROGRAMS = ${prog_1} ... ${prog_N}
bin_PROGRAMS = \
	gfsmxl_cascade_compile \
	gfsmxl_cascade_nth

EXTRA_DIST =

#bin_SCRIPTS = ${script1} ... ${scriptN}

## --- libtool libraries
#lib_LTLIBRARIES = ${lib_1}.la ... ${lib_N}.la

## --- shared data in @pkgdatadir@
#pkgdata_DATA = ${data_1} ... ${data_N}

## --- manpages -- section is auto-detected
#man_MANS = ${man_1} ... ${man_N}

##-----------------------------------------------------------------------
## sources
##-----------------------------------------------------------------------

SUFFIXES = \
	.gog _cmdparser.c _cmdparser.cc _cmdparser.h \
	.pod .txt .1 .html .tex


##-----------------------------------------------------------------------
## individual programs
##-----------------------------------------------------------------------

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
## --- Required sources
#${prog_i}_SOURCES =
##
## --- Extra (possible)sources
#EXTRA_${prog_i}_SOURCES = 
#EXTRA_${lib_i}_la_SOURCES = 
##
## --- leftovers
#mootconfig_SOURCES = mootconfig.cc
##
## --- Additional dependencies
#gfsmcompile_main.o: gfsmcompile_cmdparser.h
##
## --- Linker flags
#${prog_i}_LDFLAGS = -L. -static
#${prog_i}_LDADD   = ${non_src_file}.o -lsomelib
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#gfsmxl_X_SOURCES = \
#	gfsmxl_X_main.c \
#	gfsmxl_X_cmdparser.c gfsmxl_X_cmdparser.h
#
#gfsmxl_X_main.o: gfsmxl_X_cmdparser.h
#
#gfsmxl_X_LDFLAGS = $(LDFLAGS_COMMON)
#gfsmxl_X_LDADD = $(LDADD_COMMON)
#
#EXTRA_DIST += gfsmxl_X.gog
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
gfsmxl_cascade_compile_SOURCES = \
	gfsmxl_cascade_compile_main.c \
	gfsmxl_cascade_compile_cmdparser.c gfsmxl_cascade_compile_cmdparser.h

gfsmxl_cascade_compile_main.o: gfsmxl_cascade_compile_cmdparser.h

gfsmxl_cascade_compile_LDFLAGS = $(LDFLAGS_COMMON)
gfsmxl_cascade_compile_LDADD = $(LDADD_COMMON)

EXTRA_DIST += gfsmxl_cascade_compile.gog
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
gfsmxl_cascade_nth_SOURCES = \
	gfsmxl_cascade_nth_main.c \
	gfsmxl_cascade_nth_cmdparser.c gfsmxl_cascade_nth_cmdparser.h

gfsmxl_cascade_nth_main.o: gfsmxl_cascade_nth_cmdparser.h

gfsmxl_cascade_nth_LDFLAGS = $(LDFLAGS_COMMON)
gfsmxl_cascade_nth_LDADD = $(LDADD_COMMON)

EXTRA_DIST += gfsmxl_cascade_nth.gog
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


##-----------------------------------------------------------------------
## headers
##-----------------------------------------------------------------------

## --- installable headers: @includedir@/@pkgname@
#pkginclude_HEADERS = ${hfile_1} ... ${hfile_N}

## --- no-install headers
noinst_HEADERS = config.h

##-----------------------------------------------------------------------
## pre-compile rules: gengetopt
##-----------------------------------------------------------------------

##-----------------------------------------------------------------------
## pre-compile rules: Getopt::Gen
##-----------------------------------------------------------------------
.SECONDARY:

OPTGENFLAGS = -u -l --no-handle-rcfile

if HAVE_OPTGEN

.gog_cmdparser.h:
	optgen.perl $(OPTGENFLAGS) --nopod -F $*_cmdparser $<

.gog_cmdparser.c:
	optgen.perl $(OPTGENFLAGS) --nopod -F $*_cmdparser $<

.gog.pod:
	optgen.perl $(OPTGENFLAGS) --nocfile --nohfile -F $* $<

endif

##-----------------------------------------------------------------------
## compile flags
##-----------------------------------------------------------------------

## --- preprocessor flags
AM_CPPFLAGS = -I@srcdir@/../libgfsmxl -I../libgfsmxl $(gfsmxl_CPPFLAGS)

## --- compiler flags (c++)
AM_CFLAGS = $(gfsmxl_WFLAGS) $(gfsmxl_OFLAGS)

##-----------------------------------------------------------------------
## linker flags
##   + library special handling of configure.in vars:
##-----------------------------------------------------------------------
#${prog_i}_LDFLAGS = -L. -static
#${prog_i}_LDADD   = ${non_src_file}.o -lsomelib

LDFLAGS_COMMON = -L../libgfsmxl
LDADD_COMMON   = ../libgfsmxl/libgfsmxl.la @gfsmxl_LIBS@

#${lib_i}_la_LDFLAGS = -L. -version-info ${lib_i_current}:${lib_i_rev}:${lib_i_age}
#${lib_i}_LIBADD     = -lotherlib

##-----------------------------------------------------------------------
## cleanup: fst
##-----------------------------------------------------------------------

fsmclean:
	rm -f *.afsa *.afst *.thfsa *.thfst *.gfsa *.gfst *.gfsx *.gfsc 

##-----------------------------------------------------------------------
## Variables: cleanup
##-----------------------------------------------------------------------

## --- mostlyclean: built by 'make' & commonly rebuilt
#MOSTLYCLEANFILES =

## --- clean:  built by 'make'
#CLEANFILES =

## --- distclean: built by 'configure'
#DISTCLEANFILES = 

## -- maintainerclean: built by maintainer / by hand
MAINTAINERCLEANFILES = \
	*~ .*~ Makefile Makefile.in \
	*_cmdparser.c *_cmdparser.h \
	*.man *.1 *.html *.txt *.tex \
	*.afs[atm] *.gfs[atmxc] *.thfs[atm] *.pfs[atm] \
	*.dot *.ps *.vcg

##-----------------------------------------------------------------------
## Variables: distribution
##-----------------------------------------------------------------------

## --- extra distribution files
#EXTRA_DIST =

## --- recursion subdirectories for 'make dist'
#DIST_SUBDIRS = $(SUBDIRS)

## --- dist-hook: when another 'Makefile.am' is overkill
#DISTHOOK_DIRS = foo
#DISTHOOK_FILES = foo/bar.txt foo/baz.txt
#dist-hook:
#	for d in $(DISTHOOK_DIRS); do\
#	  mkdir -p $(distdir)/$$d ;\
#	done
#	for f in $(DISTHOOK_FILES); do\
#	  cp -p $(srcdir)/$$f $(distdir)/$$f ;\
#	done

##-----------------------------------------------------------------------
## Rules: cleanup
##-----------------------------------------------------------------------
.PHONY: cvsclean

cvsclean: maintainer-clean ;
