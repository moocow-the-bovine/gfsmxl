#!/usr/bin/perl -w

use lib qw(./blib/lib ./blib/arch);
use Gfsm;
use Gfsm::XL;


##--------------------------------------------------------------
## MAIN

##-- dummy
sub main_dummy {
  foreach $i (0..3) {
    print "--dummy($i)--\n";
  }
}
main_dummy();

