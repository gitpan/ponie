#! perl -w
# Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
# $Id: headers.pl,v 1.15 2004/03/08 08:49:05 brentdax Exp $

=head1 NAME

config/auto/headers.pl - C headers

=head1 DESCRIPTION

Probes for various C headers.

=cut

package Configure::Step;

use strict;
use vars qw($description @args);
use Parrot::Configure::Step ':auto';
use Config;

$description="Probing for C headers...";

@args=qw(miniparrot);

sub runstep {
    my ($miniparrot) = @_;

    return if $miniparrot;

    # perl5's Configure system doesn't call this by its full name, which may
    # confuse use later, particularly once we break free and start doing all
    # probing ourselves
    my %mapping =
      (
       i_niin => "i_netinetin",
      );

    for (keys %Config) {
	next unless /^i_/;
	Configure::Data->set($mapping{$_}||$_, $Config{$_});
    }

    # some headers may not be probed-for by perl 5, or might not be
    # properly reflected in %Config (i_fcntl seems to be wrong on my machine,
    # for instance).
    #
    # FreeBSD wants this order:
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    # hence add sys/types.h to the reprobe list, and have 2 goes at getting
    # the header.
    my @extra_headers = qw(malloc.h fcntl.h setjmp.h pthread.h signal.h
			   sys/types.h sys/socket.h netinet/in.h arpa/inet.h
			   sysexit.h);

    my @found_headers;
    foreach my $header (@extra_headers) {
	my $pass;

	# First try with just the header. If that fails, try with all the
	# headers we found so far. This is somewhat a hack, but makes probing
	# work on *BSD where some headers are documented as relying on others
	# being included first.
	foreach my $use_headers ([$header], [@found_headers, $header]) {
	    Configure::Data->set(testheaders =>
				 join ('',
				       map {"#include <$_>\n"} @$use_headers));
	    Configure::Data->set(testheader =>$header);

	    cc_gen('config/auto/headers/test_c.in');

	    Configure::Data->set(testheaders => undef);
	    Configure::Data->set(testheader => undef);

	    eval { cc_build(); };
	    if (!$@ && cc_run() =~ /^$header OK$/) {
		$pass = 1;
		push @found_headers, $header;
	    }
	    cc_clean();
	    last if $pass;
	}

        my $flag = "i_$header";
        $flag =~ s/\.h$//g; $flag =~ s/\///g;

	Configure::Data->set($flag, $pass ? 'define' : undef);
    }

}

1;