
use strict;
use Config;
use Data::Dumper qw(Dumper);

use FindBin;

chdir($FindBin::Bin);
print "Working in $FindBin::Bin\n";
my @config_args;
my $configs = $Config{config_argc};

use Cwd;
my $dir = cwd;

foreach my $i (1..$configs) {
  my $config_value = $Config{"config_arg$i"};
  next if($config_value =~/thread/i);
  next if($config_value eq '-Dmksymlinks');
  next if($config_value =~/\-de/);
  push @config_args, $config_value;
}

push @config_args,'-Dnoextensions=Encode';
push @config_args,'-de';
push @config_args,'-Dusedevel';
push @config_args,'-Dusethreads';
push @config_args,'-Uuseithreads';
push @config_args,'-Uuse5005threads';
push @config_args,"-Doptimize='-g'";



delete($ENV{'PERL5LIB'});

unless(-e 'parrot/parrot') {
    do_perl_configure() unless(-e 'perl/config.h');
    chdir('parrot')  || die "You need to checkout parrot inside ponie";
    chdir('classes') || die;
    system('rm -f perl5lv.pmc') && die "Error";
    system('ln','-s','../../src/pmc/perl5lv.pmc') && die "error";
    system('rm -f perl5av.pmc') && die "Error";
    system('ln','-s','../../src/pmc/perl5av.pmc') && die "error";
    system('rm -f perl5xpvhv.pmc') && die "Error";
    system('ln','-s','../../src/pmc/perl5xpvhv.pmc') && die "error";
    chdir('..') || die;
    system($^X,'Configure.pl', "--gc=libc",
	   "--ccflags= :add{ -I$dir/perl}") && die "error";
    system('make')  && die "error";
    chdir('..') || die;
}

push @config_args,'-Dlocincpth=' . $FindBin::Bin . '/parrot/include';
push @config_args,'-Dloclibpth=' . $FindBin::Bin . '/parrot/blib/lib';
push @config_args,qq{-Dotherlibdirs=lib/};
push @config_args,qq{-A},qq{append:libswanted= parrot};


do_perl_configure();

sub do_perl_configure {

    chdir('perl') || die;
    my $cmd = 'sh Configure ';
    $cmd .= join(' ',@config_args);
    print "Executing '$cmd'\n";
    

    system('rm -f Policy.sh');
    system('rm -f Policy.h');
    system('rm -f config.sh');
    system('rm -f config.h');
    system('sh','Configure',@config_args)  && die "error";;
    chdir('..');
}

