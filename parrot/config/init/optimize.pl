#! perl -w
# Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
# $Id: optimize.pl,v 1.3 2004/03/06 22:24:37 brentdax Exp $

=head1 NAME

config/init/optimize.pl - Optimization

=head1 DESCRIPTION

Enables optimization by adding the appropriate flags for the local
platform to the C<CCFLAGS>.

=cut

package Configure::Step;

use strict;
use vars qw($description @args);
use Parrot::Configure::Step;

$description="Enabling optimization...";

@args=qw(verbose);

sub runstep {
  if (Configure::Data->get('optimize')) {
    my($ccflags, $optimize) =
      Configure::Data->get(qw(ccflags optimize));
    $ccflags .= " $optimize";

    Configure::Data->set(
                         ccflags => $ccflags,
                        );
  }
  else {
    print "(none requested) " if $_[0];
  }
}

1;
