#!/usr/bin/perl -w

use lib qw(./blib/lib ./blib/arch);
use Gfsm;
use Gfsm::XL;

BEGIN { *refcnt = *__refcnt = \&Gfsm::XL::__refcnt; }

sub make {
  print STDERR "system(make)\n";
  system('make');
}

##--------------------------------------------------------------
## test 1
sub showref {
  my ($name,$ref) = @_;
  print STDERR sprintf("\$%s=%s ; \$\$%s=%#x)\n", $name, $ref, $name, ${$ref});
}

sub test1 {
  our $fsm1  = Gfsm::Automaton->new();
  showref('fsm1', $fsm1);

  our $xfsm1 = $fsm1->to_indexed();
  our $xfsm1b = $xfsm1;
  our $csc   = Gfsm::XL::Cascade->new();

  showref('xfsm1',  $xfsm1);
  showref('xfsm1b', $xfsm1b);
  showref('csc', $csc);

  $csc->_append($xfsm1);
  our $csc0  = $csc->get(0);
  our $csc0b = $csc->get(0);

  showref('csc0', $csc0);
  showref('csc0b', $csc0b);

  ##-- undefine temps
  #undef($csc);
  #undef($xfsm1);
  undef($xfsm1b);
  undef($csc0);
  undef($csc0b);
}
#test1;

##--------------------------------------------------------------
## test 2
sub test2 {
  our ($fsm0,$fsm1, $xfsm0,$xfsm1, $csc);

  $fsm0 = Gfsm::Automaton->new();
  $fsm0->compile('csc0.tfst');
  $xfsm0 = $fsm0->to_indexed;

  $fsm1 = Gfsm::Automaton->new();
  $fsm1->compile('csc1.tfst');
  $xfsm1 = $fsm1->to_indexed;

  $csc = Gfsm::XL::Cascade->new();
  $csc->_append($xfsm0,$xfsm1);

  $csc->save('test.gfsc',0);

  $csc2 = ref($csc)->new();
  $csc2->load("test.gfsc");
}
test2;

##--------------------------------------------------------------
## MAIN

##-- dummy
sub main_dummy {
  foreach $i (0..3) {
    print "--dummy($i)--\n";
  }
}
main_dummy();

