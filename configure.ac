dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.5)

dnl Some handy macros
define([THE_PACKAGE_NAME],    [gfsmxl])
define([THE_PACKAGE_VERSION], [0.0.7])
define([THE_PACKAGE_MAINTAINER],  [moocow@ling.uni-potsdam.de])

AC_INIT(THE_PACKAGE_NAME, THE_PACKAGE_VERSION, THE_PACKAGE_MAINTAINER)

dnl source && aux dir
AC_CONFIG_AUX_DIR(config)

dnl canonical target (sets $target, $target_(cpu|vendor|os) : used for bindist)
AC_CANONICAL_TARGET

dnl use automake
AM_INIT_AUTOMAKE(THE_PACKAGE_NAME, THE_PACKAGE_VERSION)

dnl use autoheader
AM_CONFIG_HEADER([src/libgfsmxl/gfsmxlConfigAuto.h])

dnl default prefix
AC_PREFIX_DEFAULT(/usr/local)

#-------------------------------------------------------------
# save user's *FLAGS
#USER_LIBS="$LIBS"
#USER_LDFLAGS="$LDFLAGS"
#USER_CPPFLAGS="$CPPFLAGS"
USER_CFLAGS="$CFLAGS"

dnl
dnl check for programs
dnl
AC_PROG_CC
dnl AC_PROG_CXX
dnl AC_LANG(C++)

##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## gnulib
dnl gl_EARLY
dnl gl_INIT
## /gnulib
## ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

##----------------------------------------------------------------------
## DISTCHECK: simulate missing development-only packages
AC_ARG_VAR(DISTCHECK,
	[(Maintainer): set to nonempty value to simulate missing development packages])
if test -n "$DISTCHECK" ; then
   test -z "$PERL"  && PERL=no
   test -z "$OPTGEN_PERL" && OPTGEN_PERL=no
   test -z "$DOXYGEN" && DOXYGEN=no
   test -z "$POD2X" && POD2TEXT=no
   test -z "$POD2X" && POD2MAN=no
   test -z "$POD2X" && POD2HTML=no
   test -z "$POD2X" && POD2LATEX=no
   test -z "$FLEX"  && FLEX=no
   test -z "$BISON" && BISON=no
fi
## /DISTCHECK
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## flex
##
AC_ARG_VAR(FLEX, [Path to GNU flex; "no" to disable])
if test -z "$FLEX" ; then
  AC_PATH_PROG(FLEX,flex,[no])
fi

AC_MSG_NOTICE([setting FLEX=$FLEX])

if test "$FLEX" = "no"; then
  AC_MSG_WARN([GNU flex is missing, broken, or disabled])
  AC_MSG_WARN([- compilation from .l lexer source files disabled])
fi

AM_CONDITIONAL(HAVE_FLEX,[test "$FLEX" != "no"])
##
## /flex
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## bison
##
AC_ARG_VAR(BISON, [Path to GNU bison; "no" to disable])
if test -z "$BISON" ; then
  AC_PATH_PROG(BISON,bison,[no])
fi

AC_MSG_NOTICE([setting BISON=$BISON])

if test "$BISON" = "no"; then
  AC_MSG_WARN([GNU bison is missing, broken, or disabled])
  AC_MSG_WARN([- compilation from .y parser source files disabled])
fi

AM_CONDITIONAL(HAVE_BISON,[test "$BISON" != "no"])
##
## /bison
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## flex+bison: automake woes

###-- automake wants these, we want flex & bison
dnl ... and these wreak havoc
dnl AM_PROG_LEX
dnl AC_PROG_YACC
dnl
dnl ... these are goofy too
dnl LEX="$FLEX"
dnl YACC="$BISON"
dnl AC_SUBST(LEX)
dnl AC_SUBST(YACC)
dnl
dnl ... with some hacking in src/libgfsm/Makefile.am, we get:
dnl     : AM_YFLAGS = --defines --fixed-output-files --name-prefix="$*_yy"
AM_PROG_LEX
AC_PROG_YACC
LEX="$FLEX"
YACC="$BISON"
## /flex+bison: automake woes
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## autotools magic, Part II
##
dnl v--- needed if Makefile.am uses _LTLIBRARIES targets
AC_PROG_LIBTOOL

dnl v--- for static-only libraries (non-libtool)
dnl AC_PROG_RANLIB

dnl v--- needed if Makefile.am uses lex sources (see above)
dnl AM_PROG_LEX

### --- unmangle user *FLAGS
if test "$CFLAGS" != "$USER_CFLAGS" ; then
  # autoconf likes to set this (to '-g -O2')... who knows why...
  AC_MSG_NOTICE([Restoring user's original CFLAGS value])
  CFLAGS="$USER_CFLAGS"
fi
## /autotools magic, Part II
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## config options: init
CONFIG_OPTIONS=""
## /config options
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## debug ?
##
AC_MSG_CHECKING([whether to build debug version])
AC_ARG_ENABLE(debug,
	AC_HELP_STRING([--enable-debug], [build debug version (default=no)]))

if test "$enable_debug" == "yes" ; then
   AC_MSG_RESULT(yes)

   dnl-- this breaks default shared-library building
   dnl-- on debian/sid:
   dnl    + automake 1.9.6
   dnl    + autoconf 2.59
   dnl    + libtool  1.5.20
   dnl
   dnl AC_DISABLE_SHARED
   if test "$GCC" == "yes" ; then
     AC_MSG_NOTICE([GNU C compiler detected: setting appropriate debug flags])
     gfsmxl_OFLAGS="-g"
   else
     AC_MSG_WARN([GNU C compiler not detected: you must use CFLAGS to set compiler debugging flags])
     gfsmxl_OFLAGS=""
   fi

   AC_DEFINE(GFSM_DEBUG_ENABLED,1,
	     [Define this to enable debugging code])
   DOXY_DEFINES="$DOXY_DEFINES GFSMXL_DEBUG_ENABLED=1"
   #CONFIG_OPTIONS="$CONFIG_OPTIONS DEBUG=1"
   CONFIG_OPTIONS="DEBUG=1"
else
  AC_MSG_RESULT(no)
  if test "$GCC" == "yes"; then
   case "$CFLAGS" in
    *-O*)
      AC_MSG_NOTICE([CFLAGS appears already to contain optimization flags - skipping])
      gfsmxl_OFLAGS=""
      ;;
    *)
     AC_MSG_NOTICE([GNU C compiler detected: setting appropriate optimization flags])
     #gfsmxl_OFLAGS="-O3 -finline-limit-10000 --param inline-unit-growth=1000"
     gfsmxl_OFLAGS="-O2"
     ;;
   esac
  else
    AC_MSG_WARN([GNU C compiler not detected: you must use CFLAGS to set compiler optimization flags])
    gfsmxl_OFLAGS=""
  fi
  #CONFIG_OPTIONS="$CONFIG_OPTIONS DEBUG=0"
  CONFIG_OPTIONS="DEBUG=0"
fi

AC_SUBST(gfsmxl_OFLAGS)
##
## /debug ?
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#---------------------------------------------------------------
# short package includes
spkgincludedir="\${includedir}/AC_PACKAGE_NAME"
AC_SUBST(spkgincludedir)
# short package includes
#---------------------------------------------------------------


#---------------------------------------------------------------
# get real prefix
AC_CACHE_CHECK([for installation prefix], [ac_cv_install_prefix],
	[if test "$prefix" = "NONE" ; then
	   ac_cv_install_prefix="/usr/local"
	 else
           ac_cv_install_prefix="$prefix"
	 fi
	])
dnl #  ... and add it to our flags
dnl CPPFLAGS="$CPPFLAGS -I${ac_cv_install_prefix}/include"
dnl LDFLAGS="$LDFLAGS -L${ac_cv_install_prefix}/lib"


dnl check for strdup
AC_CHECK_FUNC(strdup,[AC_DEFINE(HAVE_STRDUP,1,[Define this if you have the strdup() function])])


##---------------------------------------------------------------
## pkg-config : program
##
AC_ARG_VAR(PKG_CONFIG, [How to run the pkg-config program])
AC_ARG_VAR(PKG_CONFIG_PATH, [Directories to search for pkg-config])
if test -z "$PKG_CONFIG" ; then
  AC_PATH_PROG(PKG_CONFIG,pkg-config,[])
fi
##
## pkg-config: destination directory
AC_ARG_WITH(pkgconfig-dir,
	AC_HELP_STRING([--with-pkgconfig-dir=DIR],
		[install pkg-config metafile(s) in DIR (default=LIBDIR/pkgconfig)]),
	[ac_cv_pkgconfigdir="$withval"])
if test -z "$ac_cv_pkgconfigdir" ; then
  ac_cv_pkgconfigdir="\$(libdir)/pkgconfig"
fi
pkgconfigdir="$ac_cv_pkgconfigdir"
AC_SUBST(pkgconfigdir)
##
## /pkg-config
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## gfsm (pkg-config)
##
PKG_CHECK_MODULES(GFSM, gfsm >= 0.0.10,
	[PC_HAVE_GFSM="yes"],
	[PC_HAVE_GFSM=""])

if test "$PC_HAVE_GFSM" != "yes" ; then
    AC_MSG_WARN([gfsm >= v0.0.10 not found via pkg-config])
    AC_MSG_WARN([- Is the directory containing gfsm.pc in your])
    AC_MSG_WARN([  'PKG_CONFIG_PATH' environment variable?])
    AC_MSG_WARN([- I'll assume that you have set the variables CFLAGS, LDFLAGS,])
    AC_MSG_WARN([   and LIBS for compling and linking with libgfsm])
    ac_cv_enable_gfsm="no"
else
    ac_cv_enable_gfsm="yes"
fi

if test "$ac_cv_enable_gfsm" != "no" ; then
  ##-- gfsm: flags
  #CFLAGS="$CFLAGS $GFSM_CFLAGS"
  LDFLAGS="$LDFLAGS `$PKG_CONFIG --libs-only-L gfsm`"
  gfsmxl_CPPFLAGS="$gfsmxl_CPPFLAGS `$PKG_CONFIG --cflags-only-I gfsm`"
  gfsmxl_LIBS="$gfsmxl_LIBS `$PKG_CONFIG --libs-only-l gfsm`"
fi
##
## /gfsm
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## version-info
##
GFSMXL_VERSION_MAJOR=`[echo ${PACKAGE_VERSION} | sed -e's/^\([0-9][0-9]*\)\..*/\1/']`
test -z "$GFSMXL_VERSION_MAJOR" -o "$GFSMXL_VERSION_MAJOR" = "$PACKAGE_VERSION" \
  && GFSMXL_VERSION_MAJOR=0

GFSMXL_VERSION_MINOR=`[echo ${PACKAGE_VERSION} | sed -e's/^[^\.]*\.0*\([0-9][0-9]*\).*/\1/']`
test -z "$GFSMXL_VERSION_MINOR" -o "$GFSMXL_VERSION_MINOR" = "$PACKAGE_VERSION" \
  && GFSMXL_VERSION_MINOR=0

GFSMXL_VERSION_MICRO=`[echo ${PACKAGE_VERSION} | sed -e's/^[^\.]*\.[^\.]*\.0*\([0-9][0-9]*\).*/\1/']`
test -z "$GFSMXL_VERSION_MICRO" -o "$GFSMXL_VERSION_MICRO" = "$PACKAGE_VERSION" \
  && GFSMXL_VERSION_MICRO=0

AC_DEFINE_UNQUOTED(GFSMXL_VERSION_MAJOR, $GFSMXL_VERSION_MAJOR, [Major gfsmxl version])
AC_DEFINE_UNQUOTED(GFSMXL_VERSION_MINOR, $GFSMXL_VERSION_MINOR, [Minor gfsmxl version])
AC_DEFINE_UNQUOTED(GFSMXL_VERSION_MICRO, $GFSMXL_VERSION_MICRO, [Micro gfsmxl version])
AC_SUBST(GFSMXL_VERSION_MAJOR)
AC_SUBST(GFSMXL_VERSION_MINOR)
AC_SUBST(GFSMXL_VERSION_MICRO)
##
## /version-info
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## perl
AC_ARG_VAR(PERL, [Path to your perl interpreter, "no" to disable])
if test -z "$PERL" ; then
  AC_PATH_PROG(PERL,[perl],[no])
fi
if test -z "$PERL" -o "$PERL" = "no"; then
  AC_MSG_WARN([perl missing or disabled])
  AC_MSG_WARN([- generation of alternate documentation formats disabled.])
  PERL=no
fi
AC_SUBST(PERL)
## /perl
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## optgen.perl & Getopt::Gen
##
AC_ARG_VAR(OPTGEN_PERL, [Path to the 'optgen.perl' script; "no" to disable])

##-- test for working optgen.perl
AC_MSG_CHECKING([whether Getopt::Gen works])
  if test "$PERL" != "no" && $PERL -MGetopt::Gen -e'exit 0;' >>config.log 2>&1; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  AC_MSG_WARN([Getopt::Gen (or something it depends on) is broken!])
  AC_MSG_WARN([- you probably need to fix your Parse::Lex module])
  OPTGEN_PERL="no"
fi

if test -z "$OPTGEN_PERL" ; then
  AC_PATH_PROG(OPTGEN_PERL,[optgen.perl],[no])
fi

AC_MSG_NOTICE([setting OPTGEN_PERL=$OPTGEN_PERL])
AC_SUBST(OPTGEN_PERL)

if test "$OPTGEN_PERL" = "no" ; then
  AC_MSG_WARN([optgen.perl script is missing, broken, or disabled.])
  AC_MSG_WARN([- regeneration of command-line parsers from .gog specifications disabled.])
  AC_MSG_WARN([- regeneration of POD documentation from .gog specifications disabled.])
fi
AM_CONDITIONAL(HAVE_OPTGEN, [test "$OPTGEN_PERL" != "no"])
##
## /pptgen.perl
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## documentation
##
AC_ARG_WITH(docdir,
	AC_HELP_STRING([--with-docdir=DIR],
		[install documentation in DIR/AC_PACKAGE_NAME (default=DATADIR/doc)]),
	[docdir="$withval"],
	[docdir="\${datadir}/doc"])
pkgdocdir="\${docdir}/\${PACKAGE}"
pkgdocprogdir="\${docdir}/\${PACKAGE}/programs"
#pkgdoctutdir="\${docdir}/\${PACKAGE}/tutorial"
pkgdoclibdir="\${docdir}/\${PACKAGE}/lib\${PACKAGE}"

AC_SUBST(docdir)
AC_SUBST(pkgdocdir)
AC_SUBST(pkgdocprogdir)
AC_SUBST(pkgdoctutdir)
AC_SUBST(pkgdoclibdir)

AC_ARG_WITH(doc-formats,
	AC_HELP_STRING([--with-doc-formats=LIST],
		       [Build documentation formats in LIST. \
                        Available formats: txt, man, html, dvi, ps, pdf, none.
	                Default='man html'. Requires Perl, pod2xxx.]),
	[ac_cv_doc_formats="$withval"])
AC_ARG_ENABLE(doc,
	AC_HELP_STRING([--disable-doc],[Synonym for --with-doc-formats="none"]),
	[enable_doc="$enableval"],[enable_doc="yes"])

AC_MSG_CHECKING([which documentation formats to build])
## -- set default doc formats if unspecified
if test -z "$ac_cv_doc_formats" ; then
  ac_cv_doc_formats="man html"
fi

## -- un-comma-tize the doc-formats
gfsmxl_doc_formats=`echo "$ac_cv_doc_formats" | sed 's/\,/ /g'`

if test "$enable_doc" != "yes" ; then
 ##-- disable all docs
 gfsmxl_doc_formats="none"
fi
AC_MSG_RESULT($gfsmxl_doc_formats)
#echo ">> post RESULT"

##-- doxygen: var
AC_ARG_VAR(DOXYGEN,[Path to doxygen documentation generator; "no" to disable])

if test "$gfsmxl_doc_formats" != "none" ; then

  ##-- doxygen: prog
  ##
  if test -z "$DOXYGEN" ; then
    AC_PATH_PROG(DOXYGEN,doxygen,[no])
  fi
  AC_MSG_NOTICE([setting DOXYGEN=$DOXYGEN])
  ##
  if test -z "$DOXYGEN" -o "$DOXYGEN" = "no"; then
    AC_MSG_WARN([doxygen missing or disabled])
    AC_MSG_WARN([- generation of library documentation disabled])
    DOXYGEN=no
  fi
  ##
  ##--/doxygen:prog

  ##-- doxygen: filter: doc/lib*/'doxy-filter.perl'
  ##
  AC_ARG_VAR(DOXY_FILTER,[doxygen input filter; "no" to disable (default)])
  AC_MSG_CHECKING([for doxygen input filter])
  if test -z "$DOXY_FILTER"; then
    DOXY_FILTER="no"
    #DOXY_SRCDIR="`find ${srcdir}/doc/ -name 'lib*' -print | head -n1`"
    #if test -d "$DOXY_SRCDIR" -a -f "$DOXY_SRCDIR/doxy-filter.perl" -a "$PERL" != "no" ; then
    #  DOXY_SRCDIR=`cd "$DOXY_SRCDIR"; pwd`
    #  DOXY_FILTER="$PERL $DOXY_SRCDIR/doxy-filter.perl"
    #else
    #  DOXY_FILTER="no"
    #fi
  fi
  AC_MSG_RESULT([$DOXY_FILTER])
  ##
  if test "$DOXY_FILTER" != "no"; then
    AC_MSG_CHECKING([whether doxygen input filter works])
    if test -n "$DOXY_FILTER" && $DOXY_FILTER </dev/null 2>&1 >>config.log ; then
      AC_MSG_RESULT([yes])
    else
      AC_MSG_RESULT([no])
      DOXY_FILTER="no"
    fi
  fi
  AC_MSG_NOTICE([setting DOXY_FILTER=$DOXY_FILTER])
  ##
  if test "$DOXY_FILTER" != "no"; then
    DOXY_INPUT_FILTER="$DOXY_FILTER"
  else
    DOXY_INPUT_FILTER=""
  fi
  AC_SUBST(DOXY_FILTER)
  AC_SUBST(DOXY_INPUT_FILTER)
  ##
  ##--/doxygen:filter

  ##-- doxygen: sources
  AC_SUBST(DOXYGEN_SOURCES)

  ##-- docs:pod2x
  ##
  AC_ARG_VAR(POD2TEXT, [Path to perl 'pod2text' script; "no" for none])
  AC_ARG_VAR(POD2MAN,  [Path to perl 'pod2man' script; "no" for none])
  AC_ARG_VAR(POD2HTML, [Path to perl 'pod2html' script; "no" for none])
  AC_ARG_VAR(POD2LATEX,[Path to perl 'pod2latex' script; "no" for none])
  if test -z "$POD2TEXT" ; then
     AC_PATH_PROG(POD2TEXT,pod2text,[no])
  fi
  if test -z "$POD2MAN"  ; then
     AC_PATH_PROG(POD2MAN,pod2man,[no])
  fi
  if test -z "$POD2HTML" ; then
     AC_PATH_PROG(POD2HTML,pod2html,[no])
  fi
  if test -z "$POD2LATEX"; then
     AC_PATH_PROG(POD2LATEX,pod2latex,[no])
  fi
  AC_SUBST(POD2TEXT)
  AC_SUBST(POD2MAN)
  AC_SUBST(POD2HTML)
  AC_SUBST(POD2LATEX)
  ##
  ##--/docs:pod2x

  ##-- docs: .gog sources
  ##
  DOC_MAN1_GOGS="\
	gfsmxl_cascade_compile.gog \
	gfsmxl_cascade_nth.gog \
	gfsmxl_cascade_nbest_strings.gog \
	gfsmxl_cascade_nbest_lookup.gog \
	"
 DOC_MAN1_GOGS="$DOC_MAN1_GOGS"
  for g in $DOC_MAN1_GOGS ; do
    AC_CONFIG_LINKS(doc/programs/${g}:src/programs/${g})  
  done
  DOC_GOGS="$DOC_MAN1_GOGS"
  AC_SUBST(DOC_MAN1_GOGS)
  AC_SUBST(DOC_GOGS)
  dnl AC_CONFIG_LINKS(doc/programs/acknowledge.pod:src/programs/acknowledge.pod)
  ##
  ##--/docs: .gog sources

  ##-- docs: index-skeleton (section 1)
  #DOC_PROG_INDEX_SKEL="gfsmutils.skel"
  AC_SUBST(DOC_PROG_INDEX_SKEL)

  ##-- docs: section 5 (static pods)
  DOC_MAN5_PODS=""
  AC_SUBST(DOC_MAN5_PODS)

  ##-- docs: pods (all)
  DOC_PODS="\
     $DOC_MAN5_PODS \
     `echo \"$DOC_MAN1_GOGS\"       | sed 's/\.gog/\.pod/g'` \
     `echo \"$DOC_PROG_INDEX_SKEL\" | sed 's/\.skel/\.pod/g'` \
     "
  AC_SUBST(DOC_PODS)

  ##-- set "CONFIG_DOC_WANT_*" variables
  ##   + test with automake conditionals "DOC_WANT_*"
  CONFIG_DOC_WANT_TXT="no"
  CONFIG_DOC_WANT_MAN="no"
  CONFIG_DOC_WANT_HTML="no"
  CONFIG_DOC_WANT_LATEX="no"
  CONFIG_DOC_WANT_DVI="no"
  CONFIG_DOC_WANT_PS="no"
  CONFIG_DOC_WANT_PDF="no"
  
  ##-- docs: parse user request
  ##
  for fmt in $gfsmxl_doc_formats ; do
    case "$fmt" in
      txt)
        CONFIG_DOC_WANT_TXT="yes"
	;;
      man)
        CONFIG_DOC_WANT_MAN="yes"
	;;
      html)
        CONFIG_DOC_WANT_HTML="yes"
	;;
      dvi)
        CONFIG_DOC_WANT_LATEX="yes"
        CONFIG_DOC_WANT_DVI="yes"
	;;
      ps)
	CONFIG_DOC_WANT_LATEX="yes"
        CONFIG_DOC_WANT_PS="yes"
	;;
      pdf)
        CONFIG_DOC_WANT_LATEX="yes"
	CONFIG_DOC_WANT_PDF="yes"
	;;
      *)
	AC_MSG_WARN(ignoring unknown documentation format: $fmt)
	;;
    esac; # case "$fmt" in ...
  done; # for fmt in $gfsmxl_doc_formats ...
  ##
  ##--/docs: parse user request

  ##-- docs: requested: automake conditionals: indicator values
  AC_SUBST(CONFIG_DOC_WANT_TXT)
  AC_SUBST(CONFIG_DOC_WANT_MAN)
  AC_SUBST(CONFIG_DOC_WANT_HTML)
  AC_SUBST(CONFIG_DOC_WANT_LATEX)
  AC_SUBST(CONFIG_DOC_WANT_DVI)
  AC_SUBST(CONFIG_DOC_WANT_PS)
  AC_SUBST(CONFIG_DOC_WANT_PDF)

  ##-- docs: doxygen vars (compatibility)
  DOXY_WANT_MAN="$CONFIG_DOC_WANT_MAN"
  DOXY_WANT_HTML="$CONFIG_DOC_WANT_HTML"
  DOXY_WANT_LATEX="$CONFIG_DOC_WANT_LATEX"
  DOXY_WANT_HTMLHELP="NO"
  AC_SUBST(DOXY_WANT_HTML)
  AC_SUBST(DOXY_WANT_MAN)
  AC_SUBST(DOXY_WANT_LATEX)

  ##-- docs: doxygen: defines
  AC_SUBST(DOXY_DEFINES)

  ##-- doxygen: external tag-files (this needs an overhaul!)
  ##
  DOXY_TAG_EXTENSIONS="gfsm"
  for ext in $DOXY_TAG_EXTENSIONS ; do
    extdocdir="`$PKG_CONFIG --variable=pkgdocdir ${ext}`"
    if test -n "$extdocdir" -a "$extdocdir" != "no" ; then
      exttagfiles="`find $extdocdir -name '*.tag'`"
      for exttag in $exttagfiles ; do
        exttagdir="`dirname $exttag`/html"
        if test -d "$exttagdir" ; then
          DOXY_TAGFILES="$DOXY_TAGFILES $exttag=$exttagdir"
        fi
      done
    fi
  done 
  AC_SUBST(DOXY_TAGFILES)   
  ##
  ##--/doxyxgen: tag-files
  
fi; # if "$gfsmxl_doc_formats" != "none" ...

##-- conditionals
AM_CONDITIONAL(HAVE_DOXYGEN,     [test -n "$DOXYGEN"     -a "$DOXYGEN"     != "no"])
AM_CONDITIONAL(HAVE_DOXY_FILTER, [test -n "$DOXY_FILTER" -a "$DOXY_FILTER" != "no"])

AM_CONDITIONAL(HAVE_POD2TEXT,    [test -n "$POD2TEXT"  -a "$POD2TEXT"  != "no"])
AM_CONDITIONAL(HAVE_POD2MAN,     [test -n "$POD2MAN"   -a "$POD2MAN"   != "no"]) 
AM_CONDITIONAL(HAVE_POD2HTML,    [test -n "$POD2HTML"  -a "$POD2HTML"  != "no"])
AM_CONDITIONAL(HAVE_POD2LATEX,   [test -n "$POD2LATEX" -a "$POD2LATEX" != "no"])

AM_CONDITIONAL(DOC_WANT_TXT,     [test -n "$CONFIG_DOC_WANT_TXT"   -a "$CONFIG_DOC_WANT_TXT"   != "no"])
AM_CONDITIONAL(DOC_WANT_MAN,     [test -n "$CONFIG_DOC_WANT_MAN"   -a "$CONFIG_DOC_WANT_MAN"   != "no"])
AM_CONDITIONAL(DOC_WANT_HTML,    [test -n "$CONFIG_DOC_WANT_HTML"  -a "$CONFIG_DOC_WANT_HTML"  != "no"])
AM_CONDITIONAL(DOC_WANT_LATEX,   [test -n "$CONFIG_DOC_WANT_LATEX" -a "$CONFIG_DOC_WANT_LATEX" != "no"])
AM_CONDITIONAL(DOC_WANT_DVI,     [test -n "$CONFIG_DOC_WANT_DVI"   -a "$CONFIG_DOC_WANT_DVI"   != "no"])
AM_CONDITIONAL(DOC_WANT_PS,      [test -n "$CONFIG_DOC_WANT_PS"    -a "$CONFIG_DOC_WANT_PS"    != "no"])
AM_CONDITIONAL(DOC_WANT_PDF,     [test -n "$CONFIG_DOC_WANT_PDF"   -a "$CONFIG_DOC_WANT_PDF"   != "no"])

##
## /Documentation
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^



#---------------------------------------------------------------
# Binary distribution
BINDIST_RELEASE=0

## -- hack: canonicalize package-name to lower-case (for debian)
BINDIST_PKGNAME=`echo "$PACKAGE" | tr '[[:upper:]]' '[[:lower:]]'`

## -- hack: downgrade ix86 -> i386
case "$target_cpu" in
  i[[3-9]]86)
	BINDIST_CPU=i386
	;;
  *)
	BINDIST_CPU="$target_cpu"
	;;
esac
BINDIST_OS="$target_os"

AC_SUBST(BINDIST_PKGNAME)
AC_SUBST(BINDIST_RELEASE)
AC_SUBST(BINDIST_CPU)
AC_SUBST(BINDIST_OS)
# Binary distribution
#---------------------------------------------------------------


##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## warnings ?
##
AC_MSG_CHECKING([whether to display compiler warnings])
AC_ARG_ENABLE(warnings,
	AC_HELP_STRING([--disable-warnings],[disable compiler warnings (default=no)]))

if test "$enable_warnings" != "no" ; then
   AC_MSG_RESULT(yes)
   if test "$GCC" == "yes"; then
     AC_MSG_NOTICE([GNU C compiler detected: setting appropriate warning flags])
     gfsmxl_WFLAGS="-Wall -Winline"
   else
     AC_MSG_NOTICE([GNU C compiler not detected: not setting warning flags])
     gfsmxl_WFLAGS=""
   fi
else
  AC_MSG_RESULT(no)
  gfsmxl_WFLAGS=""
fi
AC_SUBST(gfsmxl_WFLAGS)
##
## /warnings
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## autotest
dnl AC_CONFIG_TESTDIR(tests,tests)
## /autotest
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

##vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
## configuration options
AC_SUBST(CONFIG_OPTIONS)
AC_SUBST(gfsmxl_LIBS)
AC_SUBST(gfsmxl_CPPFLAGS)
## /config options
##^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

dnl
dnl output
dnl
AC_CONFIG_FILES(src/libgfsmxl/Makefile)
AC_CONFIG_FILES(src/programs/Makefile)
AC_CONFIG_FILES(src/Makefile)
AC_CONFIG_FILES(doc/libgfsmxl/mainpage.dox doc/libgfsmxl/libgfsmxl.doxy doc/libgfsmxl/Makefile)
AC_CONFIG_FILES(doc/programs/Makefile)
dnl AC_CONFIG_FILES(doc/tutorial/Makefile)
AC_CONFIG_FILES(doc/Makefile)
AC_CONFIG_FILES(config/Makefile)
dnl AC_CONFIG_FILES(tests/atlocal tests/Makefile)
AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(gfsmxl.pc)
AC_OUTPUT
