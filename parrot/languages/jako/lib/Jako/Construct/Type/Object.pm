#
# Object.pm
#
# Copyright (C) 2002-2003 Gregor N. Purdy. All rights reserved.
# This program is free software. It is subject to the same license
# as the Parrot interpreter.
#
# $Id: Object.pm,v 1.3 2003/12/05 15:44:20 mrjoltcola Exp $
#

use strict;
eval "use warnings";

package Jako::Construct::Type::Object;

use base qw(Jako::Construct::Type);

sub new
{
  my $class = shift;
  my ($token) = @_;

  return bless {
    TOKEN => $token,
    CODE  => 'P',
    NAME  => 'obj',
    IMCC  => 'pmc'
  }, $class;
}

1;
