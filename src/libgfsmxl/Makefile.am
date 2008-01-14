## File: src/Makefile.am
## Package: *
## Description:
##   + source-level automake file 
##
## Process this file with Automake to create Makefile.in.
##-----------------------------------------------------------------------

##-----------------------------------------------------------------------
## subdirectories
##-----------------------------------------------------------------------
#SUBDIRS = gnulib

##-----------------------------------------------------------------------
## primary targets
##-----------------------------------------------------------------------

## --- executable programs
#bin_PROGRAMS = ${prog_1} ... ${prog_N}
#bin_PROGRAMS = dwdspp moot moot-fstgen moot-pargen

## --- libtool libraries
#lib_LTLIBRARIES = ${lib_1}.la ... ${lib_N}.la
lib_LTLIBRARIES = libgfsmxl.la

## --- shared data in @pkgdatadir@
#pkgdata_DATA = ${data_1} ... ${data_N}

## --- manpages -- section is auto-detected
#man_MANS = ${man_1} ... ${man_N}

##-----------------------------------------------------------------------
## sources
##-----------------------------------------------------------------------

## --- Required sources
#${prog_i}_SOURCES =
#${lib_i}_la_SOURCES =
libgfsmxl_la_SOURCES = \
	config.h \
	gfsmxlCascade.c \
	gfsmxlCascadeLookup.c \
	gfsmxlCLCFib.c \
	gfsmxlLabelIndex.c

## --- Extra (possible) sources
#EXTRA_${prog_i}_SOURCES = 
#EXTRA_${lib_i}_la_SOURCES = 

## --- Additional dependencies

##----------------------------------------------------
## Additional rules

gfsmxlConfigNoAuto.h: gfsmxlConfigAuto.h.in
	rm -f "$@"
	cp "$<" "$@"

##-----------------------------------------------------------------------
## headers
##-----------------------------------------------------------------------

## --- installable headers: @includedir@/@pkgname@
#pkginclude_HEADERS = ${hfile_1} ... ${hfile_N}
pkginclude_HEADERS = \
	gfsmxlConfig.h \
	gfsmxlConfigNoAuto.h \
	gfsmxlConfigAuto.h \
	\
	gfsmxlCascade.h \
	gfsmxlCascade.hi \
	gfsmxlCascadeLookup.h \
	gfsmxlCascadeLookup.hi \
	gfsmxlCLCFib.h \
	gfsmxlCLCFibPriv.h \
	gfsmxlLabelIndex.h \
	gfsmxlLabelIndex.hi \
	gfsmxl.h

## --- no-install headers
#noinst_HEADERS = nopackage.h

##-----------------------------------------------------------------------
## pre-compile rules: flex ; bison
##-----------------------------------------------------------------------
SRCDIR = @srcdir@

if HAVE_FLEX
MY_LEX    = @FLEX@
MY_LFLAGS =
else
MY_LEX    = sh $(SRCDIR)/dummy-flex.sh
MY_LFLAGS =
endif
LEX       = $(MY_LEX)
AM_LFLAGS = $(MY_LFLAGS)

if HAVE_BISON
MY_YACC   = @BISON@
MY_YFLAGS = --defines --fixed-output-files --name-prefix="$(basename $*)_yy"
else
MY_YACC   = sh $(SRCDIR)/dummy-bison.sh
MY_YFLAGS =
endif
YACC      = $(MY_YACC)
AM_YFLAGS = $(MY_YFLAGS)

SUFFIXES = .l .lex.l .y .tab.y .tab.c .tab.h .lex.c .lex.h

##-----------------------------------------------------------------------
## compile flags
##-----------------------------------------------------------------------

## --- preprocessor flags
#AM_CPPFLAGS =
#AM_CPPFLAGS = -I. -I$(SRCDIR)/gnulib
AM_CPPFLAGS = -I. $(gfsmxl_CPPFLAGS)

## --- compiler flags (cc)
AM_CFLAGS = $(gfsmxl_WFLAGS) $(gfsmxl_OFLAGS)

##-----------------------------------------------------------------------
## Additional variable: library versioning
##  + for -version-info $(CUR):$(REV):$(AGE) :
##  + base lib is created as libMYLIB.so.$(CUR).$(AGE).$(REV)
##-----------------------------------------------------------------------

## --- The most recent interface number that this library implements.
LIBCUR = $(GFSMXL_VERSION_MAJOR)

## --- The difference between the newest and oldest interfaces that this
##     library implements.  In other words, the library implements all the
##     interface numbers in the range from number `CURRENT - AGE' to
##     `CURRENT'.
LIBAGE = 0

## --- The implementation number of the CURRENT interface.
LIBREV = $(GFSMXL_VERSION_MINOR)

##-----------------------------------------------------------------------
## linker flags
##   + library special handling of configure.in vars:
##-----------------------------------------------------------------------
#${prog_i}_LDFLAGS = -L. -static
#${prog_i}_LDADD   = ${non_src_file}.o -lsomelib

#${lib_i}_la_LDFLAGS = -L. -version-info ${lib_i_current}:${lib_i_rev}:${lib_i_age}
#${lib_i}_la_LIBADD  = -lotherlib

libgfsmxl_la_LDFLAGS = -version-info $(LIBCUR):$(LIBREV):$(LIBAGE)

#libgfsmxl_la_LIBADD  = gnulib/libgnu.la @gfsm_LIBS@ $(LIBS)
libgfsmxl_la_LIBADD  = @gfsmxl_LIBS@ $(LIBS)

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
	gmon.out

##-----------------------------------------------------------------------
## Variables: distribution
##-----------------------------------------------------------------------

## --- extra distribution files
EXTRA_DIST = \
	config.h \
	dummy-flex.sh \
	dummy-bison.sh

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
