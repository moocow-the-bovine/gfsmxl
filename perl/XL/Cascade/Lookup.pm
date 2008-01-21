package Gfsm::XL::Cascade::Lookup;

use IO::File;
use Carp;

##======================================================================
## Constants
##======================================================================
our $NULL = bless \(my $x=0), 'Gfsm::XL::Cascade::Lookup';

##======================================================================
## Attributes: Wrappers
##======================================================================

BEGIN {

  ## _wrap_attribute($attrName)
  ##  + defines sub "$attrName()" for get/set
  sub _wrap_attribute {
    my $attr = shift;
    eval
    "sub $attr {
      \$_[0]->_${attr}_set(\@_[1..\$#_]) if (\$#_ > 0);
      return \$_[0]->_${attr}_get();
     }";
  }

  _wrap_attribute('cascade');
  _wrap_attribute('max_weight');
  _wrap_attribute('max_paths');
  _wrap_attribute('max_ops');
  _wrap_attribute('n_ops');
}

##======================================================================
## Operation
##======================================================================

## $result = $cl->lookup_nbest($input)
## $result = $cl->lookup_nbest($input,$result)
*lookup = \&lookup_nbest;
sub lookup_nbest {
  my ($cl,$input,$result) = @_;
  $result = Gfsm::Automaton->new() if (!$result);
  $cl->_lookup_nbest($input,$result);
  return $result;
}

1;

__END__

# Below is stub documentation for your module. You'd better edit it!

=pod

=head1 NAME

Gfsm::XL::Cascade::Lookup - libgfsmxl finite-state cascade lookup routines

=head1 SYNOPSIS

 use Gfsm;
 use Gfsm::XL;

 ##------------------------------------------------------------
 ## Constructors, etc.


=head1 DESCRIPTION

Not yet written.

=cut

########################################################################
## FOOTER
########################################################################

=pod

=head1 BUGS AND LIMITATIONS

Probably many.

=head1 SEE ALSO

Gfsm(3perl),
gfsmutils(1).


=head1 AUTHOR

Bryan Jurish E<lt>moocow@ling.uni-potsdam.deE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2005-2008 by Bryan Jurish

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.4 or,
at your option, any later version of Perl 5 you may have available.


=cut
