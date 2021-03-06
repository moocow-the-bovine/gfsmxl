# -*- Mode: CPerl -*-
# t/01_ini.t; just to load module(s) by using it (them)

$TEST_DIR = './t';
#use lib qw(../blib/lib ../blib/arch); $TEST_DIR = '.'; # for debugging

# load common subs
do "$TEST_DIR/common.plt"
  or die("could not load $TEST_DIR/common.plt");

@modules = qw(Gfsm::XL);
plan(test => scalar(@modules));

# 1--N: load submodules (1 subtest/module)
foreach $module (@modules) {
  print "Test 'use $module;'\n";
  ok(do {my $rc=eval("require $module;"); die("require $module failed: $@") if ($@); $rc});
}

print "\n";
# end of t/01_ini.t

