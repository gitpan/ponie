#
# Prefix.pm
#
# Copyright (C) 2002-2003 Gregor N. Purdy. All rights reserved.
# This program is free software. It is subject to the same license
# as the Parrot interpreter.
#
# $Id: Prefix.pm,v 1.2 2002/12/20 01:59:05 gregor Exp $
#

use strict;
eval "use warnings";

package Jako::Construct::Expression::Prefix;

use base qw(Jako::Construct::Expression);

sub new
{
  my $class = shift;
  my ($op, $right);

  return bless {
    OP    => $op,
    RIGHT => $right
  }, $class;
}

1;
