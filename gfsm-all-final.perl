#!/usr/bin/perl -w

use Gfsm;

$fsm = Gfsm::Automaton->new();
$fsm->load("-") or die("$0: load failed for automaton from STDIN: $!");
$srone = Gfsm::Semiring->new($fsm->semiring_type)->one();

foreach $q (0..($fsm->n_states-1)) {
  next if (!$fsm->has_state($q));
  next if ($fsm->is_final($q));
  $fsm->final_weight($q,$srone);
}

$fsm->save("-");
