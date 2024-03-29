#
# Number.pm
#
# Copyright (C) 2002-2003 Gregor N. Purdy. All rights reserved.
# This program is free software. It is subject to the same license
# as the Parrot interpreter.
#
# $Id: Number.pm,v 1.3 2003/11/19 06:08:56 gregor Exp $
#

use strict;
eval "use warnings";

package Jako::Construct::Type::Number;

use base qw(Jako::Construct::Type);

sub new
{
  my $class = shift;
  my ($token) = @_;

  return bless {
    TOKEN => $token,
    CODE  => 'N',
    NAME  => 'num',
    IMCC  => 'num'
  }, $class;
}

1;

