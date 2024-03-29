# Copyright (C) 2001-2003 The Perl Foundation.  All rights reserved.
# $Id: cat.pasm,v 1.3 2004/01/30 11:25:04 mikescott Exp $

=head1 NAME

examples/assembly/cat.pasm - cat-like utility

=head1 SYNOPSIS

    % ./parrot examples/assembly/cat.pasm

=head1 DESCRIPTION

Simple C<cat>-like utility to test PIO read/write. Does not use STDIO.

Echoes what you type once you hit return. 

You'll have to Ctl-C to exit.

=cut

	getstdin P0
	getstdout P1
REDO:
	readline S0, P0
	print S0
	if S0, REDO
	end
