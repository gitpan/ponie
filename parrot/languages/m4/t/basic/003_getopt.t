#!perl

# $Id: 003_getopt.t,v 1.2 2004/01/26 10:14:27 leo Exp $

use strict;

use Test::More tests => 27; 

my $real_out;
my $parrot_m4 = '../../parrot m4.pbc';


#--------------------------------------------
$real_out     = `$parrot_m4 --help 2>&1`; 
is( $real_out, << "END_OUT", '--help' );
Usage: $parrot_m4 [OPTION]... FILE

Currently only long options are available.

Operation modes:
      --help                   display this help and exit
      --version                output version information and exit

Frozen state files:
      --freeze-state=FILE      dump a frozen state to FILE at end
      --reload-state=FILE      reload a frozen state from FILE at start

END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --version 2>&1`; 
is( $real_out, << 'END_OUT', '--version' );
Parrot m4 0.0.2
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --traditional 2>&1`; 
is( $real_out, << 'END_OUT', '--traditional' );
Sorry, the option '--traditional' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --fatal-warnings 2>&1`; 
is( $real_out, << 'END_OUT', '--fatal-warnings' );
Sorry, the option '--fatal-warnings' is not implemented yet.
END_OUT

#--------------------------------------------
$real_out     = `$parrot_m4 --debug=asdf 2>&1`; 
is( $real_out, << 'END_OUT', '--debug' );
Sorry, the option '--debug' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --arglength=99 2>&1`; 
is( $real_out, << 'END_OUT', '--arglength' );
Sorry, the option '--arglength' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --error-output=/tmp/m4.err 2>&1`; 
is( $real_out, << 'END_OUT', '--error-output' );
Sorry, the option '--error-output' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --include=/tmp/m4 2>&1`; 
is( $real_out, << 'END_OUT', '--include' );
Sorry, the option '--include' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --interactive 2>&1`; 
is( $real_out, << 'END_OUT', '--interactive' );
Sorry, the option '--interactive' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --synclines 2>&1`; 
is( $real_out, << 'END_OUT', '--synclines' );
Sorry, the option '--synclines' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --prefix-builtins 2>&1`; 
is( $real_out, << 'END_OUT', '--prefix-builtins' );
Sorry, the option '--prefix-builtins' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --word-regexp='[a-z]+' 2>&1`; 
is( $real_out, << 'END_OUT', '--word-regexp' );
Sorry, the option '--word-regexp' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --hash-size=99 2>&1`; 
is( $real_out, << 'END_OUT', '--hash-size' );
Sorry, the option '--hash-size' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --nesting-limit=2 2>&1`; 
is( $real_out, << 'END_OUT', '--nesting-limit' );
Sorry, the option '--nesting-limit' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --quiet 2>&1`; 
is( $real_out, << 'END_OUT', '--quiet' );
Sorry, the option '--quiet' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --silent 2>&1`; 
is( $real_out, << 'END_OUT', '--silent' );
Sorry, the option '--silent' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --diversions=3 2>&1`; 
is( $real_out, << 'END_OUT', '--diversions' );
Sorry, the option '--diversions' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --define=name=value 2>&1`; 
is( $real_out, << 'END_OUT', '--define' );
Sorry, the option '--define' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --undefine=name 2>&1`; 
is( $real_out, << 'END_OUT', '--undefine' );
Sorry, the option '--undefine' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --trace=name 2>&1`; 
is( $real_out, << 'END_OUT', '--trace' );
Sorry, the option '--trace' is not implemented yet.
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 examples/hello.m4 2>&1`; 
is( $real_out, << 'END_OUT', 'single file' );
Hello
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 examples/hello.m4 2>&1`; 
isnt( $real_out, << 'END_OUT', 'no substitution in single file' );
Hallo
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --reload-state=examples/only_T7_0.frozen examples/hello.m4 2>&1`; 
is( $real_out, << 'END_OUT', '--reload-state' );
Hello
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --reload-state=unknown_file.frozen examples/hello.m4 2>&1`; 
is( $real_out, << 'END_OUT', 'reloading a missing file' );
'unknown_file.frozen' not found
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --asdf examples/hello.m4 2>&1`; 
is( $real_out, << 'END_OUT', 'unknown' );
unknown option: !asdf!
Hello
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --reload-state=examples/only_builtin.frozen examples/hello.m4 examples/hello.m4 2>&1`; 
is( $real_out, << 'END_OUT', '2 files' );
Hello
Hello
END_OUT


#--------------------------------------------
$real_out     = `$parrot_m4 --reload-state=examples/only_builtin.frozen examples/hello.m4 examples/hello.m4 examples/hello.m4 2>&1`; 
is( $real_out, << 'END_OUT', '3 files' );
Hello
Hello
Hello
END_OUT