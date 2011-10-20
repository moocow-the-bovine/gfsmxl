    README for package gfsmxl

    Last updated for gfsmxl v0.0.1

DESCRIPTION
    The gfsmxl package is an add-on to the libgfsm library for online lookup
    operations in cascades of indexed gfsm automata.

DEPENDENCIES
    libgfsm
        Available from: http://www.ling.uni-potsdam.de/~moocow/projects/gfsm

        Simple C library for finite state automata. Requirs at least version
        0.0.10.

    pkg-config
        Available from: http://www.freedesktop.org/software/pkgconfig/

        To build from CVS, you will also need the pkg-config autoconf macros
        which come with the source distribution of pkg-config.

    doxygen (Optional)
        Required for building library documentation. Available from:
        http://www.doxygen.org

        Tested versions 1.2.15, 1.3.8, 1.4.4.

    Perl (Optional)
        Get it from http://www.cpan.org or http://www.perl.com

        Required for re-building command-line parsers and/or non-standard
        documentation formats.

    Getopt::Gen (Optional)
        A Perl module used to generate command-line option parsers. Should
        be available from the author of this package at:

        http://www.ling.uni-potsdam.de/~moocow/projects/perl

        Tested versions 0.09, 0.10.

        Note that Getopt::Gen depends on several 3rd-party perl modules,
        including Text::Template and Parse::Lex, which are available from
        CPAN.

        Note additionally that Parse::Lex v2.15 is broken: if it gives you
        grief, use the hacked version available at:

        http://www.ling.uni-potsdam.de/~moocow/projects/perl/ParseLex-2.15-h
        acked.tar.gz

    pod2man, pod2text, pod2html, pod2latex (Optional)
        The Perl documentation converstion utilities, required for building
        the correspdonding program documentation formats, should have come
        with your Perl. These are only required if you wish to build program
        documentation formats other than the defaults which come with the
        distribution.

INSTALLATION
    Issue the following commands to the shell:

     cd PACKAGE-X.Y.Z  (or wherever you extracted the distribution)
     sh ./configure
     make
     make install

BUILD OPTIONS
    The 'configure' script supports the following options, among others:

    --enable-debug , --disable-debug
        Whether to enable verbose debugging messages. Default=no.

    See the output of `./configure --help` for details on additional
    supported options.

KNOWN BUGS
    Probably many.

AUTHOR
    Bryan Jurish <moocow.bovine@gmail.com>

