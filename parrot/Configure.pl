#! perl -w
# Copyright: 2001-2003 The Perl Foundation.  All Rights Reserved.
# $Id: Configure.pl,v 1.137 2004/03/07 09:50:29 mikescott Exp $

=head1 NAME

Configure.pl - Parrot's Configuration Script

=head1 SYNOPSIS

    % perl Configure.pl [options]

=head1 DESCRIPTION

This is Parrot's configuration script. It should be run to create the
necessary system-specific files before building Parrot.

=head2 Command-line Options

=head3 General Options

=over

=item C<--help>

Prints out a description of the options and exits.

=item C<--version>

Prints out the version number of Configure.pl and exits.

=item C<--verbose>

Tells Configure.pl to output extra information about the configuration
data it is setting.

=item C<--verbose=2>

Tells Configure.pl to output information about i<every> setting added or 
changed.

=item C<--nomanicheck>

Tells Configure.pl not to run the MANIFEST check.

=item C<--ask>

This turns on the user prompts.

=back

=head3 Compile Options

You can add and remove option values with C<< :rem{<opt>} >> and
C<< :add{<opt>} >>. For example:

    perl Configure.pl --ccflags="rem{-g} :add{-O2}"

=over

=item C<--debugging=0>

Debugging is turned on by default. Use this to disable it.

=item C<--profile>

Turn on profiled compile (gcc only for now)

Tell the compiler to do an optimization phase.

=item C<--optimize>

Tell the compiler to do an optimization phase.

=item C<--inline>

Tell Configure that the compiler supports C<inline>.

=item C<--cc=(compiler)>

Specify which compiler to use.

=item C<--ccflags=(flags)>

Use the given compiler flags.

=item C<--ccwarn=(flags)>

Use the given compiler warning flags.

=item C<--libs=(libs)>

Use the given libraries.

=item C<--link=(linker)>

Specify which linker to use.

=item C<--linkflags=(flags)>

Use the given linker flags

=item C<--ld=(linker)>

Specify which loader to use for shared libraries.

=item C<--ldflags=(flags)>

Use the given loader flags for shared libraries

=item C<--lex=(lexer)>

Specify which lexer to use.

=item C<--yacc=(parser)>

Specify which parser to use.

=item C<--define=val1[,val2]>

Generate "#define PARROT_DEF_VAL1 1" ... entries in has_header.h.
Currently needed to use inet_aton for systems that lack inet_pton:

  --define=inet_aton

=back

=head3 Parrot Options

=over

=item C<--intval=(type)>

Use the given type for C<INTVAL>.

=item C<--floatval=(type)>

Use the given type for C<FLOATVAL>.

=item C<--opcode=(type)>

Use the given type for opcodes.

=item C<--ops=(files)>

Use the given ops files.

=item C<--pmc=(files)>

Use the given PMC files.

=item C<--cgoto=0>

Don't build cgoto core. This is recommended when you are short of memory.

=item C<--jitcapable>

Use JIT system.

=item C<--execcapable>

Use JIT to emit a native executable.

=item C<--gc=(type)>

Determine the type of garbage collection. The value for C<type> should
be one of: C<gc>, C<libc>, C<malloc> or C<malloc-trace>. The default is
C<gc>.

=back

=head3 Other Options (may not be implemented)

=over

=item C<--maintainer>

Use this option if you want imcc's parser and lexer files to be
generated. Needs a working parser and lexer.

=item C<--miniparrot>

Build parrot assuming only pure ANSI C is available.

=item C<--buildicu>

Build Parrot and ICU. Runs F<icu/source/configure> with the options in
F<icu/README.parrot>.

=item C<--expnetwork>

Enable experimental networking. This is an unused option and should
probably be removed.

=back

=cut

use 5.005_02;

use strict;
use vars qw($parrot_version @parrot_version);
use lib 'lib';

use Parrot::BuildUtil;
use Parrot::Configure::RunSteps;

$| = 1;

$parrot_version = parrot_version();
@parrot_version = parrot_version();

# Handle options

my %args;

for(@ARGV) {
  my($key, $value)=/--(\w+)(?:=(.*))?/;
  $key   = 'help' unless defined $key;
  $value = 1      unless defined $value;

  for($key) {
    /version/ && do {
      my $cvsid='$Id: Configure.pl,v 1.137 2004/03/07 09:50:29 mikescott Exp $';
      print <<"END";
Parrot Version $parrot_version Configure 2.0
$cvsid
END
      exit;
    };

    /help/    && do {
      print <<"EOT";
$0 - Parrot Configure 2.0

General Options:

   --help               Show this text
   --version            Show version information
   --verbose            Output extra information
   --verbose=2          Output every setting change
   --nomanicheck        Don't check the MANIFEST

   --ask                Have Configure ask for commonly-changed info

Compile Options:

You can add and remove option values with :rem{<opt>} and :add{<opt>}
e.g. : --ccflags="rem{-g} :add{-O2}"

   --debugging=0        Disable debugging, default = 1
   --profile            Turn on profiled compile (gcc only for now)
   --optimize           Optimized compile
   --inline             Compiler supports inline

   --cc=(compiler)      Use the given compiler
   --ccflags=(flags)    Use the given compiler flags
   --ccwarn=(flags)     Use the given compiler warning flags
   --libs=(libs)        Use the given libraries
   --link=(linker)      Use the given linker
   --linkflags=(flags)  Use the given linker flags
   --ld=(linker)        Use the given loader for shared libraries
   --ldflags=(flags)    Use the given loader flags for shared libraries
   --lex=(lexer)        Use the given lexical analyzer generator
   --yacc=(parser)       Use the given parser generator

   --define=inet_aton   Quick hack to use inet_aton instead of inet_pton

Parrot Options:

   --intval=(type)      Use the given type for INTVAL
   --floatval=(type)    Use the given type for FLOATVAL
   --opcode=(type)      Use the given type for opcodes
   --ops=(files)        Use the given ops files
   --pmc=(files)        Use the given PMC files

   --cgoto=0            Don't build cgoto core - recommended when short of mem
   --jitcapable         Use JIT
   --execcapable        Use JIT to emit a native executable
   --gc=(type)          Determine the type of garbage collection
                        type=(gc|libc|malloc|malloc-trace) default is gc

Other Options (may not be implemented):

   --maintainer         Create imcc's parser and lexer files. Needs a working
                        parser and lexer.
   --miniparrot         Build parrot assuming only pure ANSI C is available
   --buildicu           Build Parrot and ICU
   --expnetwork         Enable experimental networking (unused)

EOT
      exit;
    };
    $args{$key}=$value;
  }
}

$args{debugging} = 1 unless ((exists $args{debugging}) && !$args{debugging});

print <<"END";
Parrot Version $parrot_version Configure 2.0
Copyright (C) 2001-2003 The Perl Foundation.  All Rights Reserved.

Hello, I'm Configure.  My job is to poke and prod your system to figure out
how to build Parrot.  The process is completely automated, unless you passed in
the `--ask' flag on the command line, in which case it'll prompt you for a few
pieces of info.

Since you're running this script, you obviously have Perl 5--I'll be pulling
some defaults from its configuration.
END


#Run the actual steps
Parrot::Configure::RunSteps->runsteps(%args);



print <<"END";

Okay, we're done!

You can now use `make' (or your platform's equivalent to `make') to build your
Parrot. After that, you can use `make test' to run the test suite.

Happy Hacking,
        The Parrot Team

END

exit(0);