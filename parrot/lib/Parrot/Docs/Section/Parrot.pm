# Copyright: 2004 The Perl Foundation.  All Rights Reserved.
# $Id: Parrot.pm,v 1.4 2004/03/07 10:02:41 mikescott Exp $

=head1 NAME

Parrot::Docs::Section::Parrot - Root docmentation section

=head1 SYNOPSIS

	use Parrot::Docs::Section::Parrot;
	
	Parrot::Docs::Section::Parrot->new->write_html;

=head1 DESCRIPTION

This is the top-level section for the HTML documentation for Parrot.

=head2 Methods

=over

=cut

package Parrot::Docs::Section::Parrot;

use strict;

use Parrot::Docs::Section;
@Parrot::Docs::Section::Parrot::ISA = qw(Parrot::Docs::Section);

use Parrot::Distribution;

use Parrot::Docs::Section::Info;
use Parrot::Docs::Section::Docs;
use Parrot::Docs::Section::Examples;
use Parrot::Docs::Section::PMCs;
use Parrot::Docs::Section::DynaPMCs;
use Parrot::Docs::Section::C;
use Parrot::Docs::Section::Ops;
use Parrot::Docs::Section::IMCC;
use Parrot::Docs::Section::Perl;
use Parrot::Docs::Section::Libs;
use Parrot::Docs::Section::Tools;
use Parrot::Docs::Section::BigNum;
use Parrot::Docs::Section::Languages;
use Parrot::Docs::Section::Config;
use Parrot::Docs::Section::Tests;
use Parrot::Docs::Section::EditorPlugins;

=item C<new()>

Returns a new section.

=cut

sub new
{
	my $self = shift;
	
	return $self->SUPER::new(
		'Contents', 
		'index.html', 
		'What\'s in the Parrot distribution?',
		Parrot::Docs::Section::Info->new,
		Parrot::Docs::Section::Docs->new,
		Parrot::Docs::Section::Examples->new,
		Parrot::Docs::Section::PMCs->new,
		Parrot::Docs::Section::DynaPMCs->new,
		Parrot::Docs::Section::C->new,
		Parrot::Docs::Section::Ops->new,
		Parrot::Docs::Section::IMCC->new,
		Parrot::Docs::Section::Perl->new,
		Parrot::Docs::Section::Libs->new,
		Parrot::Docs::Section::Tools->new,
		Parrot::Docs::Section::BigNum->new,
		Parrot::Docs::Section::Languages->new,
		Parrot::Docs::Section::Config->new,
		Parrot::Docs::Section::Tests->new,
		Parrot::Docs::Section::EditorPlugins->new,
	);
}

=item C<write_docs($silent, $delete)>

Writes the HTML documentation.

If C<$silent> is true then progress is not reported.

If C<$delete> is true then the contents of C<$target> will be deleted
before any HTML is written. This argument is intended to be used only
once by the root section, it is not passed to subsections.

=cut

sub write_docs
{
    my $self = shift;
    my $silent = shift || 0;
    my $delete = shift || 0;
    my $dist = Parrot::Distribution->new;

    $dist->delete_html_docs if $delete;

	$self->write_html($dist, $dist->html_docs_directory, $silent);
}

=back

=head1 SEE ALSO

C<Parrot::Docs::Section::Info>, 
C<Parrot::Docs::Section::Docs>, 
C<Parrot::Docs::Section::Examples>, 
C<Parrot::Docs::Section::PMCs>, 
C<Parrot::Docs::Section::DynaPMCs>, 
C<Parrot::Docs::Section::C>, 
C<Parrot::Docs::Section::Ops>, 
C<Parrot::Docs::Section::IMCC>, 
C<Parrot::Docs::Section::Perl>, 
C<Parrot::Docs::Section::Libs>, 
C<Parrot::Docs::Section::Tools>, 
C<Parrot::Docs::Section::BigNum>, 
C<Parrot::Docs::Section::Languages>, 
C<Parrot::Docs::Section::Config>, 
C<Parrot::Docs::Section::Tests>, 
C<Parrot::Docs::Section::EditorPlugins>.

=cut

1;