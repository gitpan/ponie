#
# Declaration.pm
#
# Copyright (C) 2002-2003 Gregor N. Purdy. All rights reserved.
# This program is free software. It is subject to the same license
# as the Parrot interpreter.
#
# $Id: Declaration.pm,v 1.2 2002/12/20 01:58:54 gregor Exp $
#

use strict;
eval "use warnings";

package Jako::Construct::Declaration;

use base qw(Jako::Construct);

sub access { return shift->{ACCESS}; }

1;

