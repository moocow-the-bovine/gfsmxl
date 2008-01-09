use ExtUtils::MakeMaker;

##----------------------------------------------------------------------
## Configuration
##----------------------------------------------------------------------
our $gfsmxl_min_version = "0.0.2";
if (system("pkg-config --atleast-version=$gfsmxl_min_version gfsmxl") != 0) {
  die("Required module gfsmxl >= v$gfsmxl_min_version not found!\n");
}

$GFSMXL_IFLAGS = join(' ', map { $s=`pkg-config --cflags-only-I $_`; chomp($s); $s } qw(gfsmxl));
$GFSMXL_LDFLAGS= join(' ', map { $s=`pkg-config --libs-only-L $_`; chomp($s); $s } qw(gfsmxl));
$GFSMXL_LIBS   = join(' ', map { $s=`pkg-config --libs-only-l $_`; chomp($s); $s } qw(gfsmxl));

##----------------------------------------------------------------------
## MAIN
##----------------------------------------------------------------------
WriteMakefile
  (
   NAME		=> 'Gfsm::XL',
   DISTNAME	=> 'gfsmxl-perl',
   AUTHOR       => 'Bryan Jurish <moocow@ling.uni-potsdam.de>',
   ABSTRACT	=> 'Perl interface to libgfsmxl finite-state cascade library',
   VERSION_FROM	=> 'XL.pm',


   ##-- user variables
   #CC           => 'c++',   ##-- your c++ compiler
   LIBS         => "$GFSMXL_LDFLAGS $GFSMXL_LIBS",      ##-- additional libraries
   DEFINE       => '-DHAVE_CONFIG_H',   ##-- additional defines
   INC          => $GFSMXL_IFLAGS,      ##-- additional includes
   CCFLAGS      => '',                  ##-- additional flags
   OBJECT       => 'XL.o GfsmXLPerl.o',

   ##-- DEBUG
   #MAKEAPERL=>1,
   #LINKTYPE=>'static',
   #MAP_TARGET=>'gfsmperl',
   #CCFLAGS=>'-g',
   ##-- /DEBUG

   ##-- stuff that probably doesn't need to be changed
   #XSOPT        => '-C++',
   TYPEMAPS     => ['perlobject.map', 'typemap'],
  );


#-----------------------------------------------------------------------
# Extensions
#-----------------------------------------------------------------------
##...

sub MY::depend {
  package MY;
  my $inherited = shift->SUPER::depend(@_);
  $inherited .= (
		 "\n\n"
		 ##-- included xs dependencies
		 ."XL.c: "
		 .(''
		   ." Cascade.xs"
		   ." Lookup.xs"
		   ." XL.xs"
		  )
		 ."\n\n"
		 ##-- c->o dependencies
		 ."XL.o: XL.c GfsmXLPerl.h\n"
		 ."\n"
		 ."GfsmXLPerl.o: GfsmXLPerl.h GfsmXLPerl.c\n"
		 ."\n"
		 ## -- more here
		 ."\n"
		);
  $inherited;
}
