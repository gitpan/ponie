#! perl -w
# Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
# $Id: headers.pl,v 1.7 2004/03/07 05:45:23 brentdax Exp $

=head1 NAME

config/init/headers.pl - Nongenerated Headers

=head1 DESCRIPTION

Uses C<ExtUtils::Manifest> to determine which headers are nongenerated.

=cut

package Configure::Step;

use strict;
use vars qw($description @args);
use Parrot::Configure::Step;
use ExtUtils::Manifest qw(maniread);

$description="Determining nongenerated header files...";

@args=();

sub runstep {
    my $inc = 'include/parrot';

    my @headers=(
	sort
	map  { m{^$inc/(.*\.h)\z} }
	keys %{maniread()}
    );

    $_ = "\$(INC)/$_" for @headers;
    my $TEMP_nongen_headers = join("\\\n	", @headers);

    Configure::Data->set(
	inc            => $inc,
	TEMP_nongen_headers => $TEMP_nongen_headers,
    );
}

1;