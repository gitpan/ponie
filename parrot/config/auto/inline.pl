# Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
# $Id: inline.pl,v 1.4 2004/03/06 22:24:30 brentdax Exp $

=head1 NAME

config/auto/inline.pl - Inline Compiler Support

=head1 DESCRIPTION

Determines whether the compiler supports C<inline>.

=cut

package Configure::Step;

use strict;
use vars qw($description @args);
use Parrot::Configure::Step ':auto';

$description="Determining if your compiler supports inline...";

@args=qw(inline verbose);

sub runstep {
    my $test;
    my ($inline, $verbose) = @_;

    if (defined $inline) {
	$test = $inline;
    }
    else {
	cc_gen('config/auto/inline/test_1.in');
	eval { cc_build(); };
	if (! $@) {
	    $test = cc_run();
	    chomp $test if $test;
	}
	cc_clean();
	if (!$test) {
	    cc_gen('config/auto/inline/test_2.in');
	    eval { cc_build(); };
	    if (! $@) {
		$test = cc_run();
		chomp $test if $test;
	    }
	    cc_clean();
	}
	if ($test) {
	    print " ($test) " if $verbose;
	}
	else {
	    print " no " if $verbose;
	    $test = '';
	}
    }

    Configure::Data->set(
	inline => $test
    );
}