#!perl
#
# get runtime info about paths
#
use 5.005_03;
use Cwd 'abs_path';

my $pconfig = $ARGV[1] || 'perl6-config';

my ($pwd, $pr);
$pwd = abs_path('.');
$pr = abs_path('../../');
open(OUT, ">$pconfig");
print OUT "# DO NOT CHANGE: this file is generated by '$0'\n";
print OUT "# run 'make' to make perl6-config\n\n";
print OUT qq{\$HERE = '$pwd';\n};
print OUT qq{\$PARROT_ROOT = '$pr';\n};
close OUT;


