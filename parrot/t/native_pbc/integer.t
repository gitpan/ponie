#! perl -w
# Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
# $Id: integer.t,v 1.5 2004/03/08 00:19:53 chromatic Exp $

=head1 NAME

t/native_pbc/integer.t - Integers

=head1 SYNOPSIS

	% perl t/native_pbc/integer.t

=head1 DESCRIPTION

Tests word-size/float-type/endian-ness for different architectures.

=cut

my $comment = <<'EOC';

s. t/native_pbc/number.t for additional comments

The file is generated by:
$ parrot -o i.pbc -a - <<EOF
> print 0x10203040
> end
> EOF

$ pdump -h i.pbc
$ mv i.pbc t/native_pbc/integer_1.pbc

EOC

use Parrot::Test tests => 4;
output_is(<<CODE, '270544960', "i386 32 bit opcode_t, 32 bit intval");
# integer_1.pbc
# HEADER => [
#        wordsize  = 4   (interpreter's wordsize    = 4)
#        int_size  = 4   (interpreter's INTVAL size = 4)
#        byteorder = 0   (interpreter's byteorder   = 0)
#        floattype = 0   (interpreter's NUMVAL_SIZE = 8)
#        no endianize, no opcode, no numval transform
#        dirformat = 1
# ]

CODE

output_is(<<CODE, '270544960', "PPC BE 32 bit opcode_t, 32 bit intval");
# integer_1.pbc
# HEADER => [
# 	wordsize  = 4	(interpreter's wordsize    = 4)
# 	int_size  = 4	(interpreter's INTVAL size = 4)
# 	byteorder = 1	(interpreter's byteorder   = 1)
# 	floattype = 0	(interpreter's NUMVAL_SIZE = 8)
# 	no endianize, no opcode, no numval transform
# 	dirformat = 1
# ]

CODE

output_is(<<CODE, '270544960', "little-endian 64-bit tru64");
#       wordsize  = 8
#       byteorder = 0
CODE

output_is(<<CODE, '270544960', "big-endian 64-bit irix");
#       wordsize  = 8
#       byteorder = 1
CODE