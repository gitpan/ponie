#!perl

# $Id: m4.pl,v 1.2 2004/01/26 10:14:01 leo Exp $

# pragmata
use strict;
use 5.008;
use lib './Perl5/lib';

use Language::m4;

exit( Language::m4::main() );
