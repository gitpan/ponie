#!./perl

BEGIN {
    chdir 't' if -d 't';
    @INC = '../lib';
    unless (find PerlIO::Layer 'perlio') {
	print "1..0 # Skip: not perlio\n";
	exit 0;
    }
}

no utf8; # needed for use utf8 not griping about the raw octets

require "./test.pl";

plan(tests => 51);

$| = 1;

open(F,"+>:utf8",'a');
print F chr(0x100).'�';
ok( tell(F) == 4, tell(F) );
print F "\n";
ok( tell(F) >= 5, tell(F) );
seek(F,0,0);
ok( getc(F) eq chr(0x100) );
ok( getc(F) eq "�" );
ok( getc(F) eq "\n" );
seek(F,0,0);
binmode(F,":bytes");
my $chr = chr(0xc4);
if (ord('A') == 193) { $chr = chr(0x8c); } # EBCDIC
ok( getc(F) eq $chr );
$chr = chr(0x80);
if (ord('A') == 193) { $chr = chr(0x41); } # EBCDIC
ok( getc(F) eq $chr );
$chr = chr(0xc2);
if (ord('A') == 193) { $chr = chr(0x80); } # EBCDIC
ok( getc(F) eq $chr );
$chr = chr(0xa3);
if (ord('A') == 193) { $chr = chr(0x44); } # EBCDIC
ok( getc(F) eq $chr );
ok( getc(F) eq "\n" );
seek(F,0,0);
binmode(F,":utf8");
ok( scalar(<F>) eq "\x{100}�\n" );
seek(F,0,0);
$buf = chr(0x200);
$count = read(F,$buf,2,1);
ok( $count == 2 );
ok( $buf eq "\x{200}\x{100}�" );
close(F);

{
    $a = chr(300); # This *is* UTF-encoded
    $b = chr(130); # This is not.

    open F, ">:utf8", 'a' or die $!;
    print F $a,"\n";
    close F;

    open F, "<:utf8", 'a' or die $!;
    $x = <F>;
    chomp($x);
    ok( $x eq chr(300) );

    open F, "a" or die $!; # Not UTF
    binmode(F, ":bytes");
    $x = <F>;
    chomp($x);
    $chr = chr(196).chr(172);
    if (ord('A') == 193) { $chr = chr(141).chr(83); } # EBCDIC
    ok( $x eq $chr );
    close F;

    open F, ">:utf8", 'a' or die $!;
    binmode(F);  # we write a "\n" and then tell() - avoid CRLF issues.
    binmode(F,":utf8"); # turn UTF-8-ness back on
    print F $a;
    my $y;
    { my $x = tell(F);
      { use bytes; $y = length($a);}
      ok( $x == $y );
  }

    { # Check byte length of $b
	use bytes; my $y = length($b);
	ok( $y == 1 );
    }

    print F $b,"\n"; # Don't upgrades $b

    { # Check byte length of $b
	use bytes; my $y = length($b);
	ok( $y == 1 );
    }

    {
	my $x = tell(F);
	{ use bytes; if (ord('A')==193){$y += 2;}else{$y += 3;}} # EBCDIC ASCII
	ok( $x == $y );
    }

    close F;

    open F, "a" or die $!; # Not UTF
    binmode(F, ":bytes");
    $x = <F>;
    chomp($x);
    $chr = v196.172.194.130;
    if (ord('A') == 193) { $chr = v141.83.130; } # EBCDIC
    ok( $x eq $chr, sprintf('(%vd)', $x) );

    open F, "<:utf8", "a" or die $!;
    $x = <F>;
    chomp($x);
    close F;
    ok( $x eq chr(300).chr(130), sprintf('(%vd)', $x) );

    open F, ">", "a" or die $!;
    if (${^OPEN} =~ /:utf8/) {
        binmode(F, ":bytes:");
    }

    # Now let's make it suffer.
    my $w;
    {
	use warnings 'utf8';
	local $SIG{__WARN__} = sub { $w = $_[0] };
	print F $a;
        ok( !($@ || $w !~ /Wide character in print/i) );
    }
}

# Hm. Time to get more evil.
open F, ">:utf8", "a" or die $!;
print F $a;
binmode(F, ":bytes");
print F chr(130)."\n";
close F;

open F, "<", "a" or die $!;
binmode(F, ":bytes");
$x = <F>; chomp $x;
$chr = v196.172.130;
if (ord('A') == 193) { $chr = v141.83.130; } # EBCDIC
ok( $x eq $chr );

# Right.
open F, ">:utf8", "a" or die $!;
print F $a;
close F;
open F, ">>", "a" or die $!;
print F chr(130)."\n";
close F;

open F, "<", "a" or die $!;
$x = <F>; chomp $x;
ok( $x eq $chr );

# Now we have a deformed file.

SKIP: {
    if (ord('A') == 193) {
	skip( "EBCDIC doesn't complain" );
    } else {
	open F, "<:utf8", "a" or die $!;
	$x = <F>; chomp $x;
	local $SIG{__WARN__} = sub { ok( 1 ) };
	eval { sprintf "%vd\n", $x };
    }
}

close F;
unlink('a');

open F, ">:utf8", "a";
@a = map { chr(1 << ($_ << 2)) } 0..5; # 0x1, 0x10, .., 0x100000
unshift @a, chr(0); # ... and a null byte in front just for fun
print F @a;
close F;

my $c;

# read() should work on characters, not bytes
open F, "<:utf8", "a";
$a = 0;
for (@a) {
    unless (($c = read(F, $b, 1) == 1)  &&
            length($b)           == 1  &&
            ord($b)              == ord($_) &&
            tell(F)              == ($a += bytes::length($b))) {
        print '# ord($_)           == ', ord($_), "\n";
        print '# ord($b)           == ', ord($b), "\n";
        print '# length($b)        == ', length($b), "\n";
        print '# bytes::length($b) == ', bytes::length($b), "\n";
        print '# tell(F)           == ', tell(F), "\n";
        print '# $a                == ', $a, "\n";
        print '# $c                == ', $c, "\n";
        print "not ";
        last;
    }
}
close F;
ok( 1 );

{
    # Check that warnings are on on I/O, and that they can be muffled.

    local $SIG{__WARN__} = sub { $@ = shift };

    undef $@;
    open F, ">a";
    binmode(F, ":bytes");
    print F chr(0x100);
    close(F);

    like( $@, 'Wide character in print' );

    undef $@;
    open F, ">:utf8", "a";
    print F chr(0x100);
    close(F);

    isnt( defined $@ );

    undef $@;
    open F, ">a";
    binmode(F, ":utf8");
    print F chr(0x100);
    close(F);

    isnt( defined $@ );

    no warnings 'utf8';

    undef $@;
    open F, ">a";
    print F chr(0x100);
    close(F);

    isnt( defined $@ );

    use warnings 'utf8';

    undef $@;
    open F, ">a";
    binmode(F, ":bytes");
    print F chr(0x100);
    close(F);

    like( $@, 'Wide character in print' );
}

{
    open F, ">:bytes","a"; print F "\xde"; close F;

    open F, "<:bytes", "a";
    my $b = chr 0x100;
    $b .= <F>;
    ok( $b eq chr(0x100).chr(0xde), "21395 '.= <>' utf8 vs. bytes" );
    close F;
}

{
    open F, ">:utf8","a"; print F chr 0x100; close F;

    open F, "<:utf8", "a";
    my $b = "\xde";
    $b .= <F>;
    ok( $b eq chr(0xde).chr(0x100), "21395 '.= <>' bytes vs. utf8" );
    close F;
}

{
    my @a = ( [ 0x007F, "bytes" ],
	      [ 0x0080, "bytes" ],
	      [ 0x0080, "utf8"  ],
	      [ 0x0100, "utf8"  ] );
    my $t = 34;
    for my $u (@a) {
	for my $v (@a) {
	    # print "# @$u - @$v\n";
	    open F, ">a";
	    binmode(F, ":" . $u->[1]);
	    print F chr($u->[0]);
	    close F;

	    open F, "<a";
	    binmode(F, ":" . $u->[1]);

	    my $s = chr($v->[0]);
	    utf8::upgrade($s) if $v->[1] eq "utf8";

	    $s .= <F>;
	    ok( $s eq chr($v->[0]) . chr($u->[0]), 'rcatline utf8' );
	    close F;
	    $t++;
	}
    }
    # last test here 49
}

{
    # [perl #23428] Somethings rotten in unicode semantics
    open F, ">a";
    binmode F, ":utf8";
    syswrite(F, $a = chr(0x100));
    close A;
    is( ord($a), 0x100, '23428 syswrite should not downgrade scalar' );
    like( $a, qr/^\w+/, '23428 syswrite should not downgrade scalar' );
}

# sysread() and syswrite() tested in lib/open.t since Fcntl is used

END {
    1 while unlink "a";
    1 while unlink "b";
}

