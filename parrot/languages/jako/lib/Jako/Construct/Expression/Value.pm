#
# Value.pm
#
# Copyright (C) 2002-2003 Gregor N. Purdy. All rights reserved.
# This program is free software. It is subject to the same license
# as the Parrot interpreter.
#
# $Id: Value.pm,v 1.3 2002/12/20 01:59:05 gregor Exp $
#

use strict;
eval "use warnings";

package Jako::Construct::Expression::Value;

use base qw(Jako::Construct::Expression);

use Jako::Token;

use Jako::Construct::Expression::Value::Literal;
use Jako::Construct::Expression::Value::Identifier;

sub new
{
  my $class = shift;
  my ($block, $token) = @_;

  confess "Block undefined." unless defined $block;
  confess "Token undefined." unless defined $token;

  return Jako::Construct::Expression::Value::Identifier->new($block, $token) if $token->kind eq 'ident';
  return Jako::Construct::Expression::Value::Literal->new($block, $token)    if $token->kind eq 'literal';

  return undef;
}

sub block { return shift->{BLOCK}; }
sub token { return shift->{TOKEN}; }
sub type  { return shift->{TYPE};  }
sub value { return shift->{VALUE}; }


#
# compile()
#
# Essentially a no-op for values (except string literals... q.v.).
#

sub compile
{
  my $self = shift;
  my ($compiler) = @_;

  return $self->value;
}

1;
