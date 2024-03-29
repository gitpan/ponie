# Copyright (C) 2001-2003 The Perl Foundation.  All rights reserved.
# $Id: mops.pasm,v 1.1 2004/03/08 11:43:41 leo Exp $

=head1 NAME

examples/assembly/mops_p.pasm - Calculate a benchmark

=head1 SYNOPSIS

    % ./parrot examples/assembly/mops_p.pasm

=head1 DESCRIPTION

Calculates a value for M ops/s (million operations per second) using
C<PerlInt> PMCs.

=cut

        new    P1, .PerlInt
        new    P2, .PerlInt
        new    P3, .PerlInt
        new    P4, .PerlInt
        new    P5, .PerlInt

        set    P2, 0
        set    P3, 1
        set    P4, 10000000

        print  "Iterations:    "
        print  P4
        print  "\n"

        # Eeevil hack.
        add    P5, P5, P4
        add    P5, P5, P4

        print  "Estimated ops: "
        print  P5
        print  "\n"

        time   N1

REDO:   sub    P4, P4, P3
        if     P4, REDO

        print  "done\n"
DONE:   time   N5

        sub    N2, N5, N1

        print  "Elapsed time:  "
        print  N2
        print  "\n"

        set    N1, P5
        div    N1, N1, N2
        set    N2, 1000000.0
        div    N1, N1, N2

        print  "M op/s:        "
        print  N1
        print  "\n"

        end

=head1 SEE ALSO

F<examples/assembly/mops.pasm>, F<examples/assembly/pmcmops.pasm>.

=cut

