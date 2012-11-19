#!/usr/bin/perl -w

use bytes;
use Gfsm;

my $infile = @ARGV ? shift : '-';
open(my $fh,"<$infile") or die("$0: open failed for '$infile': $!");
binmode($fh,':raw');
print "File: $infile\n";

## @data = runpack($template,?$len)
##  + uses globals $fh,$buf,$len
my ($buf,$len);
sub runpack {
  $len = $_[1] // length(pack($_[0]));
  read($fh,$buf,$len);
  return unpack($_[0],$buf);
}

##-- read: header
my $hpack = '(A16)(a12)(a12)(a4)LLLLL(L2)';
my @data = runpack($hpack);
my @hkeys = qw(magic version version_min flags root_id n_states n_arcs srtype sort_mask reserved2 reserved3);
@head{@hkeys} = @data;
$head{$_}     = join('.',unpack('L3',$head{$_})) foreach (qw(version version_min));

##-- header: flags
my $fbits = unpack('b*',$head{flags});
$head{"flag.is_transducer"} = substr($fbits,0,1);
$head{"flag.is_weighted"} = substr($fbits,1,1);
$head{"flag.is_deterministic"} = substr($fbits,2,1);
$head{"flag.sort_mode"} = unpack('L', pack('b*',substr($fbits,3,24).pack('C',0)));
$head{"flag.unused"} = unpack('C',pack('b*','000'.substr($fbits,length($fbits)-5)));
$head{flags} = '0x'.unpack('H*',$head{flags});
my @fkeys = map {"flag.$_"} qw(is_transducer is_weighted is_deterministic sort_mode unused);
@hkeys = map {$_ eq 'flags' ? ($_,@fkeys) : $_} @hkeys;

##-- header: interpret: semiring
my $sr   = Gfsm::Semiring->new($head{srtype});
my $zero = $sr->zero;
$head{srtype} .= " (".($sr->name).")";

##-- header: interpret: sort mode
$head{$_} .= " (".Gfsm::acmask_to_chars($head{$_}).")" foreach (qw(flag.sort_mode sort_mask));

##-- dump: header
print map {" header.$_ = $head{$_}\n"} @hkeys;

##-- read & dump: final weights
my $wlen = length(pack('f',0));
my $nfw  = runpack('L');
print "finals ($nfw):\n";
my ($fw);
foreach (0..($nfw-1)) {
  ($fw) = runpack('f',$wlen);
  print " final_weight($_) = $fw\n" if ($fw != $zero);
}

##-- read & dump: arc_table_index: arc_table
my $na = runpack('L');
print "arcs.tabx.tab ($na):\n";
my $apack = '(L2)(S2)f';
my $alen  = length(pack($apack));
my (@a,@arcs);
foreach (0..($na-1)) {
  push(@arcs, [@a = runpack($apack,$alen)]);
  print " arc($_) = ", join("\t",@a), "\n";
}

##-- read & dump: arc_table_index: index ("tabx->first")
my $nai = runpack('L');
print "arcs.tabx.first ($nai):\n";
my ($af);
my $nil = ['NONE'];
foreach (0..($nai-1)) {
  $af = runpack('L',4);
  print " first($_) = #$af (", join("\t", @{$arcs[$af]||$nil}), ")\n";
}

##-- should be at eof
if (!eof($fh)) {
  warn("$0: ERROR: file '$infile' not at EOF after reading automaton!\n");
  exit 1;
}
