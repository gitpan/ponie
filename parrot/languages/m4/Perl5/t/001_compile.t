#! perl -w

# $Id: 001_compile.t,v 1.3 2004/02/27 13:13:35 mikescott Exp $

# t/001_load.t - check module loading and create testing directory

use Test::More tests => 1;

BEGIN 
{ 
  use_ok( 'Language::m4' ); i
}

# Language::m4 is not OO
# my $object = Language::m4->new ();
# isa_ok ($object, 'Language::m4');
