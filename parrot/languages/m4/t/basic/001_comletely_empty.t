#!perl

# $Id: 001_comletely_empty.t,v 1.2 2004/01/26 10:14:27 leo Exp $

use strict;

use M4::Test tests => 2;

output_is( <<'CODE', <<'OUT', 'hello' );
CODE
OUT
