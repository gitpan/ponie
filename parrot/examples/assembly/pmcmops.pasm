# Copyright (C) 2001-2003 The Perl Foundation.  All rights reserved.
# $Id: pmcmops.pasm,v 1.3 2004/01/30 11:25:04 mikescott Exp $

=head1 NAME

examples/assembly/pmcmops.pasm - Calculate a benchmark

=head1 SYNOPSIS

    % ./parrot examples/assembly/pmcmops.pasm

=head1 DESCRIPTION

Calculates a value for M ops/s (million operations per second) using
C<PerlInt> PMCs.

=cut

        new    P2, .PerlInt
        set    P2, 0
        new    P3, .PerlInt
        set    P3, 1
        new    P4, .PerlInt
        set    P4, 100000000
        
        print  "Iterations:    "
        print  P4
        print  "\n"

        new    P1, .PerlInt
        new    P5, .PerlInt
        set    P1, 2
        mul    P5, P4, P1

        print  "Estimated ops: "
        print  P5
        print  "\n"

        time   N1

REDO:   sub    P4, P4, P3
        if     P4, REDO

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

F<examples/assembly/mops.pasm>, F<examples/assembly/mops_p.pasm>.

=cut