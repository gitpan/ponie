#
# Construct.pm
#
# Abstract base class for parsed constructs (blocks, etc.).
#
# Copyright (C) 2002-2003 Gregor N. Purdy. All rights reserved.
# This program is free software. It is subject to the same license
# as the Parrot interpreter.
#
# $Id: Construct.pm,v 1.2 2002/12/20 01:58:52 gregor Exp $
#

use strict;
eval "use warnings";

package Jako::Construct;

use base qw(Jako::Processor);

sub block  { return shift->{BLOCK};  }

1;

