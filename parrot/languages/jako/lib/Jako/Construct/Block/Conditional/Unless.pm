#
# Unless.pm
#
# Copyright (C) 2002-2003 Gregor N. Purdy. All rights reserved.
# This program is free software. It is subject to the same license
# as the Parrot interpreter.
#
# $Id: Unless.pm,v 1.4 2002/12/20 01:58:58 gregor Exp $
#

use strict;
eval "use warnings";

package Jako::Construct::Block::Conditional::Unless;

use Carp;

use base qw(Jako::Construct::Block::Conditional);

#
# new()
#

sub new
{
  my $class = shift;
  my ($block, $left, $op, $right) = @_;

  confess "Block is not defined!" unless defined $block;
  confess "Left is not defined!" unless defined $left;
  confess "Op is not defined!" unless defined $op;
  confess "Right is not defined!" unless defined $right;

  confess "Block is not!" unless UNIVERSAL::isa($block, 'Jako::Construct::Block');
  confess "Left is not Value!" unless UNIVERSAL::isa($left, 'Jako::Construct::Expression::Value');
  confess "Op is not scalar!" if ref $op;
  confess "Right is not Value!" unless UNIVERSAL::isa($right, 'Jako::Construct::Expression::Value');

  my $self = bless {
    BLOCK     => $block,

    KIND      => 'unless',
    LEFT      => $left,
    OP        => $op,
    RIGHT     => $right,

    CONTENT   => [ ]
  }, $class;

  $block->push_content($self);

  return $self;
}

1;
