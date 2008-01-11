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

  our $cl = Gfsm::XL::Cascade::Lookup->new($csc, 1.0, 1);
  showref('cl', $cl);

  ##-- test
  our $input = [1,1,2,1];
  $cl->cascade->sort_all(Gfsm::acmask_from_chars('lw'));
  our $result = $cl->lookup_nbest($input);

  undef($csc);
  undef($cl);

  print "test_lookup_1(): done.\n";
}
test_lookup_1();

##--------------------------------------------------------------
## test lookup 2 (big)
sub test_lookup_2 {
  my $edit_file = "lb-data/mootm.editor.taxi3.gfst";
  my $tagh_file = "lb-data/mootm.gfst";
  my $abet_file = "lb-data/mootm.lab";

  ##-- load & prepare: alphabet
  my $abet = Gfsm::Alphabet->new();
  $abet->load($abet_file) or die("$0: load failed for alphabet '$abet_file': $!");
  my $sym2lab = $abet->asHash();

  ##-- load & prepare: editor
  my $edit_fst = Gfsm::Automaton->new();
  $edit_fst->load($edit_file) or die("$0: load failed for '$edit_file': $!");
  my $edit_xfst = $edit_fst->to_indexed;
  $edit_xfst->arcsort(Gfsm::acmask_from_chars('lwut'));

  ##-- load & prepare: morph
  my $tagh_fst = Gfsm::Automaton->new();
  $tagh_fst->load($tagh_file) or die("$0: load failed for '$tagh_file': $!");
  my $tagh_xfst = $tagh_fst->to_indexed;
  $tagh_xfst->arcsort(Gfsm::acmask_from_chars('lwut'));

  ##-- prepare: cascade
  my $csc = Gfsm::XL::Cascade->new();
  $csc->append($edit_xfst, $tagh_xfst);

  ##-- prepare: cascade lookup-best
  our $cl = Gfsm::XL::Cascade::Lookup->new($csc, 1.0, 1);

  ##-- prepare: input
  my $input_chars = 'gieng';
  my @input_labs  = @$sym2lab{split(//,$input_chars)};

  ##-- lookup
  my $result = $cl->lookup_nbest(\@input_labs);
  $result->_connect;
  $result->_rmepsilon;
  $result->_connect;
  $result->viewps(labels=>$abet);
}
#test_lookup_2();


##--------------------------------------------------------------
## MAIN

##-- dummy
sub main_dummy {
  foreach $i (0..3) {
    print "--dummy($i)--\n";
  }
}
main_dummy();

