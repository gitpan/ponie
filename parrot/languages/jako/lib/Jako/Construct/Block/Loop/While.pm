#
# While.pm
#
# Copyright (C) 2002-2003 Gregor N. Purdy. All rights reserved.
# This program is free software. It is subject to the same license
# as the Parrot interpreter.
#
# $Id: While.pm,v 1.3 2002/12/20 01:58:59 gregor Exp $
#

use strict;
eval "use warnings";

package Jako::Construct::Block::Loop::While;

use Carp;

use base qw(Jako::Construct::Block::Loop);


#
# new()
#

sub new
{
  my $class = shift;

  confess "Expected 5 args!" unless @_ == 5;

  my ($block, $prefix, $left, $op, $right) = @_;

  my $self = bless {
    BLOCK     => $block,

    PREFIX    => $prefix,
    KIND      => 'while',
    LEFT      => $left,
    OP        => $op,
    RIGHT     => $right,

    CONTENT   => [ ]
  }, $class;

  $block->push_content($self);

  return $self;
}


1;
