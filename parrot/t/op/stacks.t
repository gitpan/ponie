#! perl -w
# Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
# $Id: stacks.t,v 1.35 2004/03/08 00:19:58 chromatic Exp $

=head1 NAME

t/op/stacks.t - Stacks

=head1 SYNOPSIS

	% perl t/op/stacks.t

=head1 DESCRIPTION

Tests for stack operations, currently C<push*>, C<push_*_c> and C<pop*>
where C<*> is not C<p>.

=cut

use Parrot::Test tests => 56;
use Test::More;

# Tests for stack operations, currently push*, push_*_c and pop*
# where * != p.

# Assembler code is partially generated by subs at bottom of file

# This defines two macros:
# fp_eq N, N, LABEL
# fp_ne N, N, LABEL
# which will conditionally branch
# to LABEL if abs(n,n) < epsilon

my $fp_equality_macro = <<'ENDOFMACRO';
.macro fp_eq(J,K,L)
	save	N0
	save	N1
	save	N2

	set	N0, .J
	set	N1, .K
	sub	N2, N1,N0
	abs	N2, N2
	gt	N2, 0.000001, .$FPEQNOK

	restore N2
	restore	N1
	restore	N0
	branch	.L
.local $FPEQNOK:
	restore N2
	restore	N1
	restore	N0
.endm
.macro fp_ne(J,K,L)
	save	N0
	save	N1
	save	N2

	set	N0, .J
	set	N1, .K
	sub	N2, N1,N0
	abs	N2, N2
	lt	N2, 0.000001, .$FPNENOK

	restore	N2
	restore	N1
	restore	N0
	branch	.L
.local $FPNENOK:
	restore	N2
	restore	N1
	restore	N0
.endm
ENDOFMACRO

###############     Tests   ##################

output_is( <<"CODE", <<'OUTPUT', "pushi & popi" );
@{[ set_int_regs( sub { $_[0]} )]}
	pushi
@{[ set_int_regs( sub {-$_[0]} )]}
@{[ print_int_regs() ]}
	popi
@{[ print_int_regs() ]}
	end
CODE
0-1-2-3-4
-5-6-7-8-9
-10-11-12-13-14
-15-16-17-18-19
-20-21-22-23-24
-25-26-27-28-29
-30-31
01234
56789
1011121314
1516171819
2021222324
2526272829
3031
OUTPUT

output_is(<<"CODE", <<OUTPUT, "pushtopi & poptopi");
@{[ set_int_regs( sub { $_[0]} )]}
	pushtopi
@{[ set_int_regs( sub {-$_[0]} )]}
	poptopi
@{[ print_int_regs() ]}
        end
CODE
0-1-2-3-4
-5-6-7-8-9
-10-11-12-13-14
-1516171819
2021222324
2526272829
3031
OUTPUT

output_is(<<"CODE", <<OUTPUT, "pushbottomi & popbottomi");
@{[ set_int_regs( sub { $_[0]} )]}
	pushbottomi
@{[ set_int_regs( sub {-$_[0]} )]}
	popbottomi
@{[ print_int_regs() ]}
        end
CODE
01234
56789
1011121314
15-16-17-18-19
-20-21-22-23-24
-25-26-27-28-29
-30-31
OUTPUT

output_is(<<"CODE", <<OUTPUT, "combining pushi with poptopi, popbottomi");
@{[ set_int_regs( sub { $_[0]} )]}
	pushi
@{[ set_int_regs( sub {-$_[0]} )]}
	poptopi
@{[ print_int_regs() ]}
        print "\\n"
        popbottomi
@{[ print_int_regs() ]}
        print "\\n"

        cleari

@{[ set_int_regs( sub { $_[0]} )]}
	pushi
@{[ set_int_regs( sub {-$_[0]} )]}
	popbottomi
@{[ print_int_regs() ]}
        print "\\n"
        poptopi
@{[ print_int_regs() ]}
        end
CODE
0-1-2-3-4
-5-6-7-8-9
-10-11-12-13-14
-1516171819
2021222324
2526272829
3031

01234
56789
1011121314
1516171819
2021222324
2526272829
3031

1617181920
2122232425
2627282930
31-16-17-18-19
-20-21-22-23-24
-25-26-27-28-29
-30-31

1617181920
2122232425
2627282930
310123
45678
910111213
1415
OUTPUT

output_is(<<"CODE", <<OUTPUT, "combining popi with pushtopi, pushbottomi");
@{[ set_int_regs( sub {$_[0]} ) ]}
	pushtopi
@{[ set_int_regs( sub {-$_[0]} ) ]}
        pushbottomi
@{[ set_int_regs( sub {0} ) ]}
@{[ print_int_regs() ]}
	print "\\n"
	popi
@{[ print_int_regs() ]}
	print "\\n"

        cleari

@{[ set_int_regs( sub {$_[0]} ) ]}
	pushbottomi
@{[ set_int_regs( sub {-$_[0]} ) ]}
        pushtopi
@{[ set_int_regs( sub {0} ) ]}
@{[ print_int_regs() ]}
	print "\\n"
	popi
@{[ print_int_regs() ]}
	end
CODE
00000
00000
00000
00000
00000
00000
00

1617181920
2122232425
2627282930
310-1-2-3
-4-5-6-7-8
-9-10-11-12-13
-14-15

00000
00000
00000
00000
00000
00000
00

01234
56789
1011121314
15-16-17-18-19
-20-21-22-23-24
-25-26-27-28-29
-30-31
OUTPUT

my ($code, $output);
for (0..1024) {
   $code .= "   set I0, $_\n";
   $code .= "   set I31, " . (1024-$_) . "\n";
   $code .= "   pushi\n";
}
for (0..1024) {
   $code .= "   popi\n";
   $code .= "   print I0\n";
   $code .= "   print \",\"\n";
   $code .= "   print I31\n";
   $code .= "   print \"\\n\"\n";
   $output .= (1024-$_) . ",$_\n";
}
$code .= "      end\n";
output_is($code, $output, "pushi & popi (deep)" );


output_is(<<"CODE", <<'OUTPUT', 'pushs & pops');
@{[ set_str_regs( sub {$_[0]%2} ) ]}
	pushs
@{[ set_str_regs( sub {($_[0]+1) %2} ) ]}
@{[ print_str_regs() ]}
	print "\\n"
	pops
@{[ print_str_regs() ]}
	print "\\n"
	end
CODE
10101010101010101010101010101010
01010101010101010101010101010101
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'pushtops & poptops');
@{[ set_str_regs( sub {$_[0]%2} ) ]}
	pushtops
@{[ set_str_regs( sub {($_[0]+1) %2} ) ]}
@{[ print_str_regs() ]}
	print "\\n"
	poptops
@{[ print_str_regs() ]}
	print "\\n"
	end
CODE
10101010101010101010101010101010
10101010101010100101010101010101
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'pushbottoms & popbottoms');
@{[ set_str_regs( sub {$_[0]%2} ) ]}
	pushbottoms
@{[ set_str_regs( sub {($_[0]+1) %2} ) ]}
@{[ print_str_regs() ]}
	print "\\n"
	popbottoms
@{[ print_str_regs() ]}
	print "\\n"
	end
CODE
10101010101010101010101010101010
01010101010101011010101010101010
OUTPUT

output_is(<<"CODE", <<OUTPUT, "combining pushs with poptops, popbottoms");
@{[ set_str_regs( sub {$_[0]%2} ) ]}
	pushs
@{[ set_str_regs( sub {($_[0]+1) %2} ) ]}
@{[ print_str_regs() ]}
	print "\\n"
	poptops
@{[ print_str_regs() ]}
	print "\\n"
	popbottoms
@{[ print_str_regs() ]}
	print "\\n"

        clears

@{[ set_str_regs( sub {$_[0]%2} ) ]}
	pushs
@{[ set_str_regs( sub {($_[0]+1) %2} ) ]}
@{[ print_str_regs() ]}
	print "\\n"
	popbottoms
@{[ print_str_regs() ]}
	print "\\n"
	poptops
@{[ print_str_regs() ]}
	print "\\n"

	end
CODE
10101010101010101010101010101010
10101010101010100101010101010101
01010101010101010101010101010101
10101010101010101010101010101010
01010101010101011010101010101010
01010101010101010101010101010101
OUTPUT

output_is(<<"CODE", <<OUTPUT, "combining pops with pushtops, pushbottoms");
@{[ set_str_regs( sub {$_[0]+1} ) ]}
	pushtops
@{[ set_str_regs( sub {-$_[0]-1} ) ]}
        pushbottoms
@{[ set_str_regs( sub {0} ) ]}
@{[ print_str_regs() ]}
	print "\\n"
	pops
@{[ print_str_regs() ]}
	print "\\n"

        clears

@{[ set_str_regs( sub {$_[0]+1} ) ]}
	pushbottoms
@{[ set_str_regs( sub {-$_[0]-1} ) ]}
        pushtops
@{[ set_str_regs( sub {0} ) ]}
@{[ print_str_regs() ]}
	print "\\n"
	pops
@{[ print_str_regs() ]}
	print "\\n"
	end
CODE
00000000000000000000000000000000
17181920212223242526272829303132-1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16
00000000000000000000000000000000
12345678910111213141516-17-18-19-20-21-22-23-24-25-26-27-28-29-30-31-32
OUTPUT

($code, $output) = ();
for (0..1024) {
   $code .= "   set S0, \"$_\"\n";
   $code .= "   set S31, \"" . (1024-$_) . "\"\n";
   $code .= "   pushs\n";
}
for (0..1024) {
   $code .= "   pops\n";
   $code .= "   print S0\n";
   $code .= "   print S31\n";
   $code .= "   print \"\\n\"\n";
   $output .= (1024-$_) . "$_\n";
}
$code .= "      end\n";
output_is($code, $output, "pushs & pops (deep)" );


output_is(<<"CODE", <<'OUTPUT', 'pushn & popn');
@{[ set_num_regs( sub { "1.0".$_ } ) ]}
	pushn
@{[ set_num_regs( sub { "-1.0".$_} ) ]}
@{[ clt_num_regs() ]}
	print "Seem to have negative Nx\\n"
	popn
@{[ cgt_num_regs() ]}
	print "Seem to have positive Nx after pop\\n"
	branch ALLOK
ERROR:	print "not ok\\n"
@{[ print_num_regs() ]}
ALLOK:	end
CODE
Seem to have negative Nx
Seem to have positive Nx after pop
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'pushtopn & poptopn');
@{[ set_num_regs( sub { "1.0".$_ } ) ]}
	pushtopn
@{[ set_num_regs( sub { "-1.0".$_} ) ]}
	poptopn
@{[ check_num_regs(1) ]}
	print "all ok\\n"
	branch ALLOK
ERROR:	print "not ok\\n"
@{[ print_num_regs() ]}
ALLOK:	end
CODE
all ok
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'pushbottomn & popbottomn');
@{[ set_num_regs( sub { "1.0".$_ } ) ]}
	pushbottomn
@{[ set_num_regs( sub { "-1.0".$_} ) ]}
	popbottomn
@{[ check_num_regs(0) ]}
	print "all ok\\n"
	branch ALLOK
ERROR:	print "not ok\\n"
@{[ print_num_regs() ]}
ALLOK:	end
CODE
all ok
OUTPUT

output_is(<<"CODE", <<OUTPUT, "combining pushn with poptopn, popbottomn");
@{[ set_num_regs( sub { "1.0".$_ } ) ]}
	pushn
@{[ set_num_regs( sub { "-1.0".$_} ) ]}
	poptopn
@{[ check_num_regs(1) ]}
@{[ set_num_regs( sub { "-1.0".$_} ) ]}
	popbottomn
@{[ check_num_regs(0) ]}

	print "ok 1\\n"
        clearn

@{[ set_num_regs( sub { "1.0".$_ } ) ]}
	pushn
@{[ set_num_regs( sub { "-1.0".$_} ) ]}
	popbottomn
@{[ check_num_regs(0) ]}
@{[ set_num_regs( sub { "-1.0".$_} ) ]}
	poptopn
@{[ check_num_regs(1) ]}

	print "ok 2\\n"
	branch ALLOK
ERROR:	print "not ok\\n"
@{[ print_num_regs() ]}
ALLOK:	end
CODE
ok 1
ok 2
OUTPUT

output_is(<<"CODE", <<OUTPUT, "combining popn with pushtopn, pushbottomn");
@{[ $fp_equality_macro ]}
@{[ set_num_regs( sub { "1.0".$_ } ) ]}
	pushtopn
@{[ set_num_regs( sub { "-1.0".$_} ) ]}
        pushbottomn
        popn
@{[ check_num_regs(0) ]}
        .fp_ne(N15, 1.031, ERROR)
        .fp_ne(N16, -1.000, ERROR)

        print "ok 1\\n"

@{[ set_num_regs( sub { "1.0".$_ } ) ]}
	pushbottomn
@{[ set_num_regs( sub { "-1.0".$_} ) ]}
        pushtopn
        popn
@{[ check_num_regs(0) ]}
        .fp_ne(N15, 1.015, ERROR)
        .fp_ne(N16, -1.016, ERROR)

	print "ok 2\\n"
	branch ALLOK
ERROR:	print "not ok\\n"
@{[ print_num_regs() ]}
ALLOK:	end
CODE
ok 1
ok 2
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'pushp & popp');
	new	P0, .PerlString
	set	P0, "BUTTER IN HELL!\\n"
	pushp
	new	P0, .PerlString
	set	P0, "THERE'LL BE NO "
	print	P0
	popp
	print	P0
	end
CODE
THERE'LL BE NO BUTTER IN HELL!
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'pushtopp & poptopp');
@{[ set_pmc_regs( sub {$_[0]%2} ) ]}
	pushtopp
@{[ set_pmc_regs( sub {($_[0]+1) %2} ) ]}
@{[ print_pmc_regs() ]}
	print "\\n"
	poptopp
@{[ print_pmc_regs() ]}
	print "\\n"
	end
CODE
10101010101010101010101010101010
10101010101010100101010101010101
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'pushbottomp & popbottomp');
@{[ set_pmc_regs( sub {$_[0]%2} ) ]}
	pushbottomp
@{[ set_pmc_regs( sub {($_[0]+1) %2} ) ]}
@{[ print_pmc_regs() ]}
	print "\\n"
	popbottomp
@{[ print_pmc_regs() ]}
	print "\\n"
	end
CODE
10101010101010101010101010101010
01010101010101011010101010101010
OUTPUT

output_is(<<"CODE", <<OUTPUT, "combining pushp with poptopp, popbottomp");
@{[ set_pmc_regs( sub {$_[0]%2} ) ]}
	pushp
@{[ set_pmc_regs( sub {($_[0]+1) %2} ) ]}
@{[ print_pmc_regs() ]}
	print "\\n"
	poptopp
@{[ print_pmc_regs() ]}
	print "\\n"
	popbottomp
@{[ print_pmc_regs() ]}
	print "\\n"

        clearp

@{[ set_pmc_regs( sub {$_[0]%2} ) ]}
	pushp
@{[ set_pmc_regs( sub {($_[0]+1) %2} ) ]}
@{[ print_pmc_regs() ]}
	print "\\n"
	popbottomp
@{[ print_pmc_regs() ]}
	print "\\n"
	poptopp
@{[ print_pmc_regs() ]}
	print "\\n"

	end
CODE
10101010101010101010101010101010
10101010101010100101010101010101
01010101010101010101010101010101
10101010101010101010101010101010
01010101010101011010101010101010
01010101010101010101010101010101
OUTPUT

output_is(<<"CODE", <<OUTPUT, "combining popp with pushtopp, pushbottomp");
@{[ set_pmc_regs( sub {$_[0]+1} ) ]}
	pushtopp
@{[ set_pmc_regs( sub {-$_[0]-1} ) ]}
        pushbottomp
@{[ set_pmc_regs( sub {0} ) ]}
@{[ print_pmc_regs() ]}
	print "\\n"
	popp
@{[ print_pmc_regs() ]}
	print "\\n"

        clearp

@{[ set_pmc_regs( sub {$_[0]+1} ) ]}
	pushbottomp
@{[ set_pmc_regs( sub {-$_[0]-1} ) ]}
        pushtopp
@{[ set_pmc_regs( sub {0} ) ]}
@{[ print_pmc_regs() ]}
	print "\\n"
	popp
@{[ print_pmc_regs() ]}
	print "\\n"
	end
CODE
00000000000000000000000000000000
17181920212223242526272829303132-1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16
00000000000000000000000000000000
12345678910111213141516-17-18-19-20-21-22-23-24-25-26-27-28-29-30-31-32
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'poptopp, aggregate PMCs');
        new     P15, .PerlHash
        new     P16, .PerlHash
        set     P15["two"], 2
        set     P16["three"], 3
        pushp
        new     P15, .PerlHash
        new     P16, .PerlHash
        set     P15["two"], 3
        set     P16["three"], 2
	poptopp
        set     I0, P15["two"]
        set     I1, P16["three"]
        print   I0
        print   "\\n"
        print   I1
        print   "\\n"
        end
CODE
3
3
OUTPUT

($code, $output) = ();
for (0..1024) {
   $code .= "   new P0, .PerlString\n";
   $code .= "   new P31, .PerlString\n";
   $code .= "   set P0, \"$_\"\n";
   $code .= "   set P31, \"" . (1024-$_) . "\"\n";
   $code .= "   pushp\n";
}
for (0..1024) {
   $code .= "   popp\n";
   $code .= "   print P0\n";
   $code .= "   print P31\n";
   $code .= "   print \"\\n\"\n";
   $output .= (1024-$_) . "$_\n";
}
$code .= "      end\n";
my $SPEEDUP = $ENV{RUNNING_MAKE_TEST} ? "gc_debug 0\n" : "";
output_is($SPEEDUP . $code, $output, "pushp & popp (deep)" );


# Test proper stack chunk handling
output_is(<<CODE, <<'OUTPUT', 'save_i & restore_i');
	set     I3, 1

testloop:
	set     I0, 0
	set     I1, I3

saveloop:
	inc     I0
	save    I0
	ne      I0, I1, saveloop

restoreloop:
	restore I0
	ne      I0, I1, error
	dec	I1
	ne      I1, 0, restoreloop

	add     I3, I3, 1
	ne      I3, 769, testloop	# At least 3 stack chunks

	print	"OK\\n"
error:	end
CODE
OK
OUTPUT


# Now, to make it do BAD THINGS!
output_is(<<"CODE",'No entries on IntReg_Stack!','ENO I frames');
	popi
	end
CODE
output_is(<<"CODE",'No entries on NumReg_Stack!','ENO N frames');
	popn
	end
CODE
output_is(<<"CODE",'No entries on StringReg_Stack!','ENO S frames');
	pops
	end
CODE

# Rotate
output_is(<<"CODE", <<'OUTPUT', 'rotate 0');
    set I0, 1
    save I0
    set I0, 2
    save I0
    rotate_up 0
    restore I0
    print I0
    print "\\n"
    end
CODE
2
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'rotate 1');
    set I0, 1
    save I0
    set I0, 2
    save I0
    rotate_up 1
    restore I0
    print I0
    print "\\n"
    end
CODE
2
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'rotate 2');
    set I0, 1
    save I0
    set I0, 2
    save I0
    rotate_up 2
    restore I0
    print I0
    print "\\n"
    end
CODE
1
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'rotate 3');
    set I0, 1
    save I0
    set I0, 2
    save I0
    set I0, 3
    save I0
    rotate_up 3
    restore I0
    print I0
    restore I0
    print I0
    restore I0
    print I0
    print "\\n"
    end
CODE
213
OUTPUT


output_is(<<"CODE", <<'OUTPUT', 'rotate -1');
    set I0, 1
    save I0
    set I0, 2
    save I0
    set I0, 3
    save I0
    rotate_up -1
    restore I0
    print I0
    restore I0
    print I0
    restore I0
    print I0
    print "\\n"
    end
CODE
321
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'rotate -2');
    set I0, 1
    save I0
    set I0, 2
    save I0
    set I0, 3
    save I0
    rotate_up -2
    restore I0
    print I0
    restore I0
    print I0
    restore I0
    print I0
    print "\\n"
    end
CODE
231
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'rotate -3');
    set I0, 1
    save I0
    set I0, 2
    save I0
    set I0, 3
    save I0
    rotate_up -3
    restore I0
    print I0
    restore I0
    print I0
    restore I0
    print I0
    print "\\n"
    end
CODE
132
OUTPUT

output_is(<<'CODE', <<'OUTPUT', 'rotate with a full stack chunk');
      set I0, 0
FOO:  save I0
      inc I0
      lt I0, 256, FOO

      rotate_up 2

      restore I1
      print I1
      print "\n"
      end
CODE
254
OUTPUT

output_is(<<'CODE', <<'OUTPUT', 'rotate across stack chunk boundary');
      set I0, 0
FOO:  save I0
      inc I0
      lt I0, 257, FOO

      rotate_up 2

      restore I1
      print I1
      print "\n"
      end
CODE
255
OUTPUT

output_is(<<'CODE', <<'OUTPUT', 'rotate by stack chunk size');
      set I0, 0
FOO:  save I0
      inc I0
      lt I0, 300, FOO

      rotate_up -256

      restore I1
      print I1
      print "\n"
      end
CODE
44
OUTPUT

output_is(<<'CODE', <<'OUTPUT', 'rotate by more than stack chunk size');
      set I0, 0
FOO:  save I0
      inc I0
      lt I0, 300, FOO

      rotate_up -257

      restore I1
      print I1
      print "\n"
      end
CODE
43
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'rotate up by more than stack size');
    set I0, 1
    save I0
    set I0, 2
    save I0
    rotate_up 3
    end
CODE
Stack too shallow!
OUTPUT

output_is(<<"CODE", <<'OUTPUT', 'rotate down by more than stack size');
    set I0, 1
    save I0
    set I0, 2
    save I0
    rotate_up -3
    end
CODE
Stack too shallow!
OUTPUT

output_is(<<'CODE', <<'OUTPUT', 'save/savec for strings');
      set S0, "Foobar"
      savec S0
      chopn S0, 3
      print S0
      print "\n"
      restore S2
      print S2
      print "\n"

      set S1, "Foobar"
      save  S1
      chopn S1, 3
      print S1
      print "\n"
      restore S3
      print S3
      print "\n"
      end
CODE
Foo
Foobar
Foo
Foo
OUTPUT

output_is(<<CODE, <<OUTPUT, "save, restore");
@{[ $fp_equality_macro ]}
	set	I0, 1
	save	I0
	set	I0, 2
	print	I0
	print	"\\n"
	restore	I0
	print	I0
	print	"\\n"

	set	N0, 1.0
	save	N0
	set	N0, 2.0
	.fp_eq	(N0, 2.0, EQ1)
	print	"not "
EQ1:	print	"equal to 2.0\\n"
	restore	N0
	.fp_eq	(N0, 1.0, EQ2)
	print	"not "
EQ2:	print	"equal to 1.0\\n"

	set	S0, "HONK\\n"
	save	S0
	set	S0, "HONK HONK\\n"
	print	S0
	restore	S0
	print	S0

	save	123
	restore	I0
	print	I0
	print	"\\n"

	save	3.14159
	restore	N0
	.fp_eq	(N0, 3.14159, EQ3)
	print	"<kansas> not "
EQ3:	print	"equal to PI\\n"

	save	"All the world's people\\n"
	restore	S0
	print	S0

	new	P0, .PerlString
	set	P0, "never to escape\\n"
	save	P0
	new	P0, .PerlString
	set	P0, "find themselves caught in a loop\\n"
	print	P0
	restore	P0
	print	P0

	end
CODE
2
1
equal to 2.0
equal to 1.0
HONK HONK
HONK
123
equal to PI
All the world's people
find themselves caught in a loop
never to escape
OUTPUT

output_is(<<CODE, <<OUTPUT, "entrytype");
	set	I0, 12
	set	N0, 0.1
	set	S0, "Difference Engine #2"
	new	P0, .PerlString
	set	P0, "Shalmaneser"

	save	P0
	save	S0
	save	"Wintermute"
	save	N0
	save	1.23
	save	I0
	save	12

	print	"starting\\n"

	set	I1, 0
LOOP:	entrytype	I0, I1
	print	I0
	print	"\\n"
	inc	I1
	lt	I1, 7, LOOP

	print	"done\\n"
	end
CODE
starting
1
1
2
2
3
3
4
done
OUTPUT

output_is(<<CODE, <<OUTPUT, "entrytype, beyond stack depth");
	save	12
	print	"ready\\n"
	entrytype	I0, 1
	print	"done\\n"
	end
CODE
ready
Stack Depth wrong
OUTPUT

output_is(<<'CODE', <<'OUTPUT', "depth op");
        depth I0
        print I0
        print "\n"

        save "Foo"
        depth I0
        print I0
        print "\n"
        restore S0

        set I1, 0
LOOP:   save I1
        inc I1
        lt I1, 1024, LOOP
        depth I0
        print I0
        print "\n"

        end
CODE
0
1
1024
OUTPUT

output_is(<<'CODE', <<'OUTPUT', "intstack");
	intsave -1
	intsave 0
	intsave 1
	intsave 2
	intsave 3
	set I0, 4
	intsave I0

	intrestore I1
	print I1

	intrestore I1
	print I1

	intrestore I1
	print I1

	intrestore I1
	print I1

	intrestore I1
	print I1

	intrestore I1
	print I1

	print "\n"
	end
CODE
43210-1
OUTPUT

output_is(<<'CODE', <<'OUTPUT', "intstack stress test");
        set I0, 0
LOOP:	intsave I0
        inc I0
        lt I0, 2048, LOOP

LOOP2:  dec I0
        intrestore I1
        ne I0, I1, ERROR
        gt I0, 0, LOOP2
        print "ok\n"
        end

ERROR:  print "Not ok\n"
        end

CODE
ok
OUTPUT

output_is(<<'CODE', <<'OUTPUT', "intdepth");
        intdepth I0
        print I0
        print "\n"

        intsave 1
        intdepth I0
        print I0
        print "\n"
        intrestore I2

        set I1, 0
LOOP:   intsave I1
        inc I1
        lt I1, 1024, LOOP
        intdepth I0
        print I0
        print "\n"

        end
CODE
0
1
1024
OUTPUT

output_is(<<'CODE', <<'OUTPUT', "saveall/restoreall");
        set S0,"test ok"
        set N0,4.3
        set I0,6
        saveall
        set S0,"test not ok"
        set N0,4.1
        set I0,8
        restoreall
        print I0
        print "\n"
        print N0
        print "\n"
        print S0
        print "\n"
        end
CODE
6
4.300000
test ok
OUTPUT

output_is(<<"CODE", <<'OUTPUT', "savetop/restoretop");
@{[ $fp_equality_macro ]}
        new P15, .PerlHash
        new P16, .PerlHash
        set I15, 1
        set I16, 2
        set N15, 1.0
        set N16, 2.0
        set S15, "one"
        set S16, "two"
        set P15["one"], 1
        set P16["two"], 2

        savetop

        new P15, .PerlHash
        new P16, .PerlHash
        set I15, 3
        set I16, 4
        set N15, 3.0
        set N16, 4.0
        set S15, "three"
        set S16, "four"
        set P15["three"], 3
        set P16["four"], 4

        restoretop

        ne I15, 3, ERROR
        ne I16, 2, ERROR
        .fp_ne(N15, 3.0, ERROR)
        .fp_ne(N16, 2.0, ERROR)
        ne S15, "three", ERROR
        ne S16, "two", ERROR
        set I1, P15["three"]
        ne I1, 3, ERROR
        set I2, P16["two"]
        ne I2, 2, ERROR

        print "all ok\\n"
        branch DONE

ERROR:  print "not ok\\n"
        print I15
        print "\\n"
        print I16
        print "\\n"
        print N15
        print "\\n"
        print N16
        print "\\n"
        print S15
        print "\\n"
        print S16
        print "\\n"
        print I1
        print "\\n"
        print I2
        print "\\n"

DONE:   end
CODE
all ok
OUTPUT

$code = $fp_equality_macro;
$output = "";
for (0..1024) {
   $code .= "      set N0, $_\n";
   $code .= "      set N31, " . (1024-$_) . "\n";
   $code .= "      pushn\n";
}
for (0..1024) {
   $code .= "      popn\n";
   $code .= "      .fp_ne(N0," . (1024-$_) . ", FAIL)\n";
   $code .= "      print \"ok$_\\n\"\n";
   $code .= "      .fp_ne(N31, $_, FAIL)\n";
   $code .= "      print \"ok$_\\n\"\n";
   $output .= "ok$_\n";
   $output .= "ok$_\n";
}
$code .= "      end\n";
$code .= "FAIL: end\n";
output_is($code, $output, "pushn & popn (deep)" );


output_is(<<CODE, <<'OUTPUT', "lookback");
@{[ $fp_equality_macro ]}
        save 1
        save 1.0
        save "Foo"

        new P12, .PerlHash
        set P12["Apple"], "Banana"
        save P12

        lookback P0, 0
        lookback S0, 1
        lookback N0, 2
        lookback I0, 3

        set S2, P0["Apple"]
        eq S2, "Banana", OK1
        print "not "
OK1:    print "ok 1\\n"

        eq I0, 1, OK2
        print "not "
OK2:    print "ok 2\\n"

        .fp_eq (N0, 1.0, OK3)
        print "not "
OK3:    print "ok 3\\n"

        eq S0, "Foo", OK4
        print "not "
OK4:    print "ok 4\\n"

        lookback I1, -1
        lookback N1, -2
        lookback S1, -3
        lookback P1, -4

        eq I0, 1, OK5
        print "not "
OK5:    print "ok 5\\n"

        .fp_eq (N0, 1.0, OK6)
        print "not "
OK6:    print "ok 6\\n"

        eq S0, "Foo", OK7
        print "not "
OK7:    print "ok 7\\n"

        set S3, P1["Apple"]
        eq S3, "Banana", OK8
        print "not "
OK8:    print "ok 8\\n"

        end
CODE
ok 1
ok 2
ok 3
ok 4
ok 5
ok 6
ok 7
ok 8
OUTPUT

output_is(<<CODE, <<'OUTPUT', "check limit - User");
lp:
	save I0
	branch lp
	end
CODE
Stack 'User' too deep
OUTPUT

output_is(<<CODE, <<'OUTPUT', "check limit - Pad");
lp:
	new_pad 0
	branch lp
	end
CODE
Stack 'Pad' too deep
OUTPUT

output_is(<<CODE, <<'OUTPUT', "check limit - Control");
lp:
	bsr lp
	end
CODE
Stack 'Control' too deep
OUTPUT

##############################

# set integer registers to some value given by $code...
package main;
sub set_int_regs {
  my $code = shift;
  my $rt;
  for (0..31) {
    $rt .= "\tset I$_, ".&$code($_)."\n";
  }
  return $rt;
}
# print all integer registers, with newlines every five registers
sub print_int_regs {
  my ($rt, $foo);
  for (0..31) {
    $rt .= "\tprint I$_\n";
    $rt .= "\tprint \"\\n\"\n" unless ++$foo % 5;
  }
  $rt .= "\tprint \"\\n\"\n";
  return $rt;
}

# Set all string registers to values given by &$_[0](reg num)
sub set_str_regs {
  my $code = shift;
  my $rt;
  for (0..31) {
    $rt .= "\tset S$_, \"".&$code($_)."\"\n";
  }
  return $rt;
}
# print string registers, no additional prints
sub print_str_regs {
  my $rt;
  for (0..31) {
    $rt .= "\tprint S$_\n";
  }
  return $rt;
}

# Create a PerlString in each PMC reg, with value given by &$_[0](reg num)
sub set_pmc_regs {
  my $code = shift;
  my $rt;
  for (0..31) {
    $rt .= "\tnew P$_, .PerlString\n";
    $rt .= "\tset P$_, \"".&$code($_)."\"\n";
  }
  return $rt;
}

# print PMC registers, no additional prints
sub print_pmc_regs {
  my $rt;
  for (0..31) {
    $rt .= "\tprint P$_\n";
  }
  return $rt;
}

# Set "float" registers, &$_[0](reg num) should return string
sub set_num_regs {
  my $code = shift;
  my $rt;
  for (0..31) {
    $rt .= "\tset N$_, ".&$code($_[0])."\n";
  }
  return $rt;
}
# rather than printing all num regs, compare all ge 0
# if any are less, jump to ERROR
# sense of test may seem backwards, but isn't
sub cgt_num_regs {
  my $rt;
  for (0..31) {
#    $rt .= "\tlt_n_nc_ic N$_, 0.0, ERROR\n";
    $rt .= "\tlt N$_, 0.0, ERROR\n";
  }
  return $rt;
}
# same, but this time lt 0
sub clt_num_regs {
  my $rt;
  for (0..31) {
#    $rt .= "\tgt_n_nc_ic N$_, 0.0, ERROR\n";
    $rt .= "\tgt N$_, 0.0, ERROR\n";
  }
  return $rt;
}

# Compare num regs with zero -- half should be less than zero, half
# should be greater than zero. Pass in $lower to determine which half.
# If $lower is true, it's the lower half; otherwise, it's the upper half

sub check_num_regs {
  my $lower = shift;
  my $rt;

# lower half negative
  if ($lower) {
    for (0..15) {
      $rt .= "\tgt N$_, 0.0, ERROR\n";
    }
    for (16..31) {
      $rt .= "\tlt N$_, 0.0, ERROR\n";
    }
  }
# upper half negative
  else {
    for (0..15) {
      $rt .= "\tlt N$_, 0.0, ERROR\n";
    }
    for (16..31) {
      $rt .= "\tgt N$_, 0.0, ERROR\n";
    }
  }
  return $rt;
}

sub print_num_regs {
  my $rt;
  for (0..31) {
    $rt .= "\tprint N$_\n";
  }
  return $rt;
}
