#! perl -w
# Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
# $Id: gc.t,v 1.10 2004/03/09 09:30:40 leo Exp $

=head1 NAME

t/op/gc.t - Garbage Collection

=head1 SYNOPSIS

	% perl t/op/gc.t

=head1 DESCRIPTION

Tests garbage collection with the C<interpinfo> operation.

=cut

use Parrot::Test tests => 10;

output_is( <<'CODE', '1', "sweep 1" );
      interpinfo I1, 2   # How many DOD runs have we done already?
      sweep 1
      interpinfo I2, 2   # Should be one more now
      sub I3, I2, I1
      print I3
      end
CODE

output_is( <<'CODE', '0', "sweep 0" );
      interpinfo I1, 2   # How many DOD runs have we done already?
      sweep 0
      interpinfo I2, 2   # Should be same
      sub I3, I2, I1
      print I3
      end
CODE

output_is( <<'CODE', '1', "sweep 0, with object that need destroy" );
      interpinfo I1, 2   # How many DOD runs have we done already?
      new P0, .PerlUndef
      needs_destroy P0
      sweep 0
      interpinfo I2, 2   # Should be one more now
      sub I3, I2, I1
      print I3
      end
CODE

output_is( <<'CODE', '10', "sweep 0, with object that need destroy/destroy");
      interpinfo I1, 2   # How many DOD runs have we done already?
      new P0, .PerlUndef
      needs_destroy P0
      new P0, .PerlUndef # kill object
      sweep 0
      interpinfo I2, 2   # Should be one more now
      sub I3, I2, I1
      sweep 0
      interpinfo I4, 2   # Should be same as last
      sub I5, I4, I2
      print I3           # These create PMCs that need early DOD, so we need
      print I5           # to put them after the second sweep op.
      end
CODE

output_is( <<'CODE', '1', "collect" );
      interpinfo I1, 3   # How many garbage collections have we done already?
      collect
      interpinfo I2, 3   # Should be one more now
      sub I3, I2, I1
      print I3
      end
CODE

output_is( <<'CODE', <<'OUTPUT', "collectoff/on" );
      interpinfo I1, 3
      collectoff
      collect
      interpinfo I2, 3
      sub I3, I2, I1
      print I3
      print "\n"

      collecton
      collect
      interpinfo I4, 3
      sub I6, I4, I2
      print I6
      print "\n"

      end
CODE
0
1
OUTPUT

output_is( <<'CODE', <<'OUTPUT', "Nested collectoff/collecton" );
      interpinfo I1, 3
      collectoff
      collectoff
      collecton
      collect           # This shouldn't do anything...    #'
      interpinfo I2, 3
      sub I3, I2, I1
      print I3
      print "\n"

      collecton
      collect           # ... but this should
      interpinfo I4, 3
      sub I6, I4, I2
      print I6
      print "\n"

      end
CODE
0
1
OUTPUT

output_is(<<'CODE', <<OUTPUT, "sweepoff with newpmcs");
    print "starting\n"

    sweepoff
    set I0, 0

LOOP: new P0, .PerlString
    set P0, "ABC"
    save P0
    inc I0
    lt I0, 127, LOOP

    print "ending\n"

    end
CODE
starting
ending
OUTPUT

output_is(<<'CODE', <<OUTPUT, "vanishing slingleton PMC");
_main:
    newsub P0, .Sub, _rand
    set I16, 100
    set I17, 0
loop:
    sweep 1
    savetop
    invokecc
    restoretop
    inc I17
    lt I17, I16, loop
    print "ok\n"
    end

_rand:
    new P16, .Random
    set I5, P16[10]
    gt I5, 10, err
    lt I5, 0, err
    invoke P1
err:
    print "singleton destroyed .Random = ."
    new P16, .Random
    typeof S16, P16
    print S16
    print "\n"
    end
CODE
ok
OUTPUT

output_is(<<'CODE', <<OUTPUT, "vanishing return continuation in method calls");
    newclass P1, "Foo"

    find_global P2, "_init"
    store_global "Foo", "__init", P2
    find_global P2, "_inc"
    store_global "Foo", "__increment", P2

    find_type I1, "Foo"
    new P3, I1
    print "ok\n"
    end

.pcc_sub _init:
    set P16, P1
    print "init\n"
    sweep 1
    new P6, .PerlString
    set P6, "hi"
    newsub P0, .Sub, _do_inc
    savetop
    invokecc
    restoretop
    sweep 1
    set P1, P16
    invoke P1

_do_inc:
    sweep 1
    inc P2
    sweep 1
    print "back from _inc\n"
    invoke P1

.pcc_sub _inc:
    print "inc\n"
    sweep 1
    invoke P1
CODE
init
inc
back from _inc
ok
OUTPUT