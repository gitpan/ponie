# Copyright: 2004 The Perl Foundation.  All Rights Reserved.
# $Id: EditorPlugins.pm,v 1.3 2004/03/07 10:02:41 mikescott Exp $

=head1 NAME

Parrot::Docs::Section::EditorPlugins - Editor Plugins docmentation section

=head1 SYNOPSIS

	use Parrot::Docs::Section::EditorPlugins;
	
	my $section = Parrot::Docs::Section::EditorPlugins->new;

=head1 DESCRIPTION

A documentation section describing Parrot-related editor plugins.

=head2 Methods

=over

=cut

package Parrot::Docs::Section::EditorPlugins;

use strict;

use Parrot::Docs::Section;
@Parrot::Docs::Section::EditorPlugins::ISA = qw(Parrot::Docs::Section);

=item C<new()>

Returns a new section.

=cut

sub new
{
	my $self = shift;
	
	return $self->SUPER::new(
		'Editor Plugins', 'editor.html', '',
		$self->new_item('', 'editor')
	);
}

=back

=cut

1;