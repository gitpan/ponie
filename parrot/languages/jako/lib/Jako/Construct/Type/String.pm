#
# String.pm
#
# Copyright (C) 2002-2003 Gregor N. Purdy. All rights reserved.
# This program is free software. It is subject to the same license
# as the Parrot interpreter.
#
# $Id: String.pm,v 1.2 2002/12/20 01:59:10 gregor Exp $
#

use strict;
eval "use warnings";

package Jako::Construct::Type::String;

use base qw(Jako::Construct::Type);

sub new
{
  my $class = shift;
  my ($token) = @_;

  return bless {
    TOKEN => $token,
    CODE  => 'S',
    NAME  => 'str',
    IMCC  => 'string'
  }, $class;
}

1;
