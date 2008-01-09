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
#test2;

##--------------------------------------------------------------
## test: lookup: 1
sub test_lookup_1 {
  our $csc = Gfsm::XL::Cascade->new();
  showref('csc',$csc);

  if (1) {
    our $fsm0 = Gfsm::Automaton->new();
    our $fsm1 = Gfsm::Automaton->new();
    $fsm0->compile('csc0.tfst');
    $fsm1->compile('csc1.tfst');
    $csc->append($fsm0,$fsm1);
  }

  our $cl = Gfsm::XL::Cascade::Lookup->new($csc);
  showref('cl', $cl);

  $cl->_cascade_set($csc); ##-- repeat until crash:
  # *** glibc detected *** /usr/local/bin/perl: free(): invalid pointer: 0x0857db90 ***
  # ======= Backtrace: =========
  # /lib/i686/cmov/libc.so.6[0xb7e5e735]
  # /lib/i686/cmov/libc.so.6(cfree+0x90)[0xb7e621a0]
  # /usr/local/lib/perl/5.8.8/auto/Gfsm/Gfsm.so(gfsm_perl_free+0x1d)[0xb7a7001d]
  # /usr/lib/libglib-2.0.so.0(g_free+0x31)[0xb797b961]
  # /usr/lib/libglib-2.0.so.0(g_array_free+0x5c)[0xb795614c]
  # /usr/local/lib/libgfsmxl.so.0(gfsmxl_cascade_clear+0xd7)[0xb79fe5e7]
  # ./blib/arch/auto/Gfsm/XL/XL.so[0xb7f8a7a7]
  # ./blib/arch/auto/Gfsm/XL/XL.so[0xb7f8ad3e]
  # ./blib/arch/auto/Gfsm/XL/XL.so(gfsmxl_perl_cascade_lookup_set_cascade_sv+0x12d)[0xb7f8ace7]
  # ./blib/arch/auto/Gfsm/XL/XL.so(XS_Gfsm__XL__Cascade__Lookup__cascade_set+0x270)[0xb7f89b6a]
  # /usr/local/bin/perl(Perl_pp_entersub+0x313)[0x80c0923]
  # /usr/local/bin/perl(Perl_runops_standard+0x1b)[0x80bf2fb]
  # /usr/local/bin/perl(perl_run+0x25c)[0x806719c]
  # /usr/local/bin/perl(main+0x112)[0x8063752]
  # /lib/i686/cmov/libc.so.6(__libc_start_main+0xe0)[0xb7e09450]
  # /usr/local/bin/perl[0x80635d1]
  ##--
  #$cl->_cascade_set(undef); $cl->_cascade_set($csc); ##-- repeat with NO crash..
  ##
  ##-- IDEA: probably related to $csc-identity: use newSV() and SvSetSV() instead of mortalcopy() ?



  undef($csc);
  undef($cl);

  print "test_lookup_1(): done.\n";
}
test_lookup_1();


##--------------------------------------------------------------
## MAIN

##-- dummy
sub main_dummy {
  foreach $i (0..3) {
    print "--dummy($i)--\n";
  }
}
main_dummy();

