#!perl

# $Id: 004_define.t,v 1.2 2004/01/26 10:14:27 leo Exp $

use strict;

use M4::Test tests => 2;

output_is( <<'CODE', <<'OUT', 'hello' );
define(`foo', `Hello World')
define(`furcht', `Hallo Welt')
In German foo is furcht.
CODE


In German Hello World is Hallo Welt.
OUT
