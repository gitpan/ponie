#! perl -w
# Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
# $Id: env.pl,v 1.4 2004/03/06 22:24:30 brentdax Exp $

=head1 NAME

config/auto/env.pl - System Environment

=head1 DESCRIPTION

Determining if the C library has C<setenv()> and C<unsetenv()>.

=cut

package Configure::Step;

use strict;
use vars qw($description @args);
use Parrot::Configure::Step ':auto';

$description="Determining if your C library has setenv / unsetenv...";
@args=qw(verbose);

sub runstep {
    my ($setenv, $unsetenv) = (0, 0);

    if ($^O eq 'Win32') {
	$setenv = 1;
	$unsetenv = 1;
    }
    else {
	cc_gen('config/auto/env/test_setenv.in');
	eval { cc_build(); };
	unless ($@ || cc_run() !~ /ok/) {
	    $setenv = 1;
	}
	cc_clean();
	cc_gen('config/auto/env/test_unsetenv.in');
	eval { cc_build(); };
	unless ($@ || cc_run() !~ /ok/) {
	    $unsetenv = 1;
	}
	cc_clean();
    }

    Configure::Data->set(
	setenv => $setenv,
	unsetenv => $unsetenv
    );

    if ($setenv && $unsetenv) {
	print " (both) " if $_[0];
    }
    elsif ($setenv) {
	print " (setenv) " if $_[0];
    }
    elsif ($unsetenv) {
	print " (unsetenv) " if $_[0];
    }
    else {
	print " (no) " if $_[0];
    }
}

1;