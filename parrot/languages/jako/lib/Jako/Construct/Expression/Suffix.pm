#
# Suffix.pm
#
# Copyright (C) 2002-2003 Gregor N. Purdy. All rights reserved.
# This program is free software. It is subject to the same license
# as the Parrot interpreter.
#
# $Id: Suffix.pm,v 1.2 2002/12/20 01:59:05 gregor Exp $
#

use strict;
eval "use warnings";

package Jako::Construct::Expression::Suffix;

use base qw(Jako::Construct::Expression);

sub new
{
  my $class = shift;
  my ($left, $op);

  return bless {
    LEFT  => $left,
    OP    => $op
  }, $class;
}

1;
