#! perl -w
# Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
# $Id: byteorder.pl,v 1.2 2004/02/26 00:43:02 mikescott Exp $

=head1 NAME

config/auto/byteorder.pl - Native Byteorder

=head1 DESCRIPTION

Computes the native byteorder for Parrot's wordsize.

=cut

package Configure::Step;

use strict;
use vars qw($description @args);

$description="Computing native byteorder for Parrot's wordsize...";

@args=();

sub runstep {
  cc_gen('config/auto/byteorder/test_c.in');
  cc_build();
  my $byteorder=cc_run() or die "Can't run the byteorder testing program: $!";
  cc_clean();

  chomp $byteorder;

  if($byteorder =~ /^1234/) {
    Configure::Data->set(
      byteorder => $byteorder,
      bigendian => 0
    );
  }
  elsif($byteorder =~ /^(8765|4321)/) {
    Configure::Data->set(
      byteorder => $byteorder,
      bigendian => 1
    );
  }
  else {
    die "Unsupported byte-order [$byteorder]!";
  }
}

1;