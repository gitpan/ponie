# Copyright (C) 2001-2003 The Perl Foundation.  All rights reserved.
# $Id: trace.pasm,v 1.2 2004/01/30 11:25:05 mikescott Exp $

=head1 NAME

examples/assembly/trace.pasm - Tracing

=head1 SYNOPSIS

    % ./parrot examples/assembly/trace.pasm

=head1 DESCRIPTION

Shows you what happens when you turn on tracing.

=cut

print "Howdy!\n"
trace 1
print "There!\n"
trace 0
print "Partner!\n"
end