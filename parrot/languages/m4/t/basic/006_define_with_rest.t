#!perl

# $Id: 006_define_with_rest.t,v 1.2 2004/01/26 10:14:27 leo Exp $

use strict;

use M4::Test tests => 4;

{
  output_is( <<'CODE', <<'OUT', 'hello' );
define(`foo', `Hello World, heute ist der 15. August')
define(`furcht', `Hallo Welt')sagt regina und fangt Fliegen
In German foo is furcht.
CODE

sagt regina und fangt Fliegen
In German Hello World, heute ist der 15. August is Hallo Welt.
OUT
}
{
  output_is( <<'CODE', <<'OUT', 'hello' );
:::asdf
define(`foo', `Hello World, heute ist der 15. August')
define(`furcht', `Hallo Welt')sagt regina und fangt Fliegen
In German foo is furcht.
CODE
:::asdf

sagt regina und fangt Fliegen
In German Hello World, heute ist der 15. August is Hallo Welt.
OUT
}
