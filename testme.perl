#!/usr/bin/perl -w

use lib qw(./blib/lib ./blib/arch);
use Gfsm;
use Gfsm::XL;
use Time::HiRes qw(tv_interval gettimeofday);
use Storable;

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
    our $abet = Gfsm::Alphabet->new();
    $abet->fromArray(['<eps>', 'a', 'b']);
    our $fsm0 = Gfsm::Automaton->new();
    our $fsm1 = Gfsm::Automaton->new();
    #$fsm0->compile('csc0.tfst');
    $fsm0->compile('csc0b.tfst');
    $fsm1->compile('csc1.tfst');
    $csc->append($fsm0,$fsm1);
  }

  our $cl = Gfsm::XL::Cascade::Lookup->new($csc, 1.0, 1);
  showref('cl', $cl);

  ##-- test
  our $input = [1,1,2,1];
  $cl->cascade->sort_all(Gfsm::acmask_from_chars('lw'));
  our $result = $cl->lookup_nbest($input);
  #$result->viewps(labels=>$abet);

  undef($csc);
  undef($cl);

  print "test_lookup_1(): done.\n";
}
#test_lookup_1();

##--------------------------------------------------------------
## test lookup 2 (big)
sub dolookup_str {
  my ($cl,$abet,$str,%args) = @_;
  my @labs = @{$abet->asHash}{split(//,$str)};
  $cl->max_weight($args{max_weight}) if (defined($args{max_weight}));
  $cl->max_paths($args{max_paths}) if (defined($args{max_paths}));
  $cl->max_ops($args{max_ops}) if (defined($args{max_ops}));
  my $result = $cl->lookup_nbest(\@labs);
  return $result;
}

sub test_lookup_2 {
  my $CREATE_CASCADE=0;
  my $edit_file = "lb-data/mootm.editor.taxi3.gfst";
  my $tagh_file = "lb-data/mootm.gfst";
  my $abet_file = "lb-data/mootm.lab";
  my $csc_file  = "lb-data/mootm.taxi3.gfsc";

  ##-- load & prepare: alphabet
  my $abet = Gfsm::Alphabet->new();
  $abet->load($abet_file) or die("$0: load failed for alphabet '$abet_file': $!");

  my $csc = Gfsm::XL::Cascade->new();

  if ($CREATE_CASCADE) {
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
    $csc->save($csc_file);
  } else {
    $csc->load($csc_file) or die("$0: load failed for '$csc_file': $!");
  }

  ##-- prepare: cascade lookup-best
  our $cl    = Gfsm::XL::Cascade::Lookup->new($csc, 1.0, 1);

  ##-- lookup
  my %opts   = (max_weight=>4.0, max_paths=>1, max_ops=>(-1));
  my $result = dolookup_str($cl,$abet,'gieng',%opts);
  $result->_connect;
  $result->_rmepsilon;
  $result->_connect;
  $result->viewps(labels=>$abet);
}
#test_lookup_2();

##--------------------------------------------------------------
## test Gfsm::Automaton::lookup() vs. Gfsm::XL::Cascade::lookup_nbest()
##
sub bench_lookup_vs_nbest {
  my $xfsmfile = "hacks/mootm.gfsx";
  my $abetfile = "hacks/mootm.lab";
  my $tfile    = "hacks/negra-1k.t";

  my $abet = Gfsm::Alphabet->new();
  $abet->load($abetfile) or die("$0: load failed for alphabet file '$abetfile': $!");

  my $xfsm = Gfsm::Automaton::Indexed->new();
  $xfsm->load($xfsmfile) or die("$0: load failed for indexed automaton file '$xfsmfile': $!");
  $xfsm->arcsort(Gfsm::acmask_from_chars('lwut'));
  my $fsm = $xfsm->to_automaton();

  my $csc = Gfsm::XL::Cascade->new();
  $csc->append($xfsm);
  my $lb = Gfsm::XL::Cascade::Lookup->new($csc);
  #$lb->max_weight(Gfsm::Semiring->new($xfsm->semiring_type)->zero);
  $lb->max_weight(Gfsm::Semiring->new($xfsm->semiring_type)->one);
  $lb->max_paths(-1);
  $lb->max_ops(-1);

  ##-- debug: binary I/O on Lookup object (looks OK)
  #my $frz = Storable::freeze($lb);
  #my $lb2 = Storable::thaw($frz);

  ##-- get tokens
  open(TFILE,"<$tfile") or die("$0: open failed for test file '$tfile': $!");
  my @toks = grep { $_ !~ /^\s*$/ } map { chomp; $_ } <TFILE>;
  close(TFILE);

  my $tps_fsm = dobench_lookup('fsm',    \@toks, $abet, sub { return $fsm->lookup(@_) });
  my $tps_lb  = dobench_lookup('csc:lb', \@toks, $abet, sub { return $lb->lookup_nbest(@_); });

  print STDERR
    (
     sprintf("%8s: %8.2f tok/sec\n", 'fsm',    $tps_fsm), ##-- ~14831 tok/sec
     sprintf("%8s: %8.2f tok/sec\n", 'csc:lb', $tps_lb),  ##-- ~ 6553 tok/sec
    );
}
bench_lookup_vs_nbest();

sub dobench_lookup {
  my ($name,$toks,$abet,$sub) = @_;
  my ($sym2id,$tok,$result,@labs,$paths);
  $sym2id = $abet->asHash;
  $result = Gfsm::Automaton->new();
  my $started = [gettimeofday];
  my $ntoks   = @$toks;
  foreach $tok (@$toks) {
    @labs = @$sym2id{split(//,$tok)};
    $result = $sub->(\@labs,$result);
    #$result->_connect();
    $paths  = $result->paths;
  }
  return $ntoks / tv_interval($started);
}

##--------------------------------------------------------------
## MAIN

##-- dummy
sub main_dummy {
  foreach $i (0..3) {
    print "--dummy($i)--\n";
  }
}
main_dummy();

