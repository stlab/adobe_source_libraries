#!/usr/bin/perl

use warnings ;
use strict ;

use Cwd;
use FindBin qw($Bin);

use lib "$Bin/../../adobe_source_libraries/tools";
use Adobe::ShellUtils qw(run_perl_script);


### Establish ASL root directory:
my $ASL_ROOT=Cwd::realpath("$Bin/../..");
my $DOCS=Cwd::realpath("$ASL_ROOT/adobe_source_libraries/documentation");

#my $filter = "^.*\.html\$";
my $filter = ".*";
my $command  = "doxygen doxyfile";
my $srcdir = Cwd::realpath("$DOCS");
my $targdir = Cwd::realpath("$DOCS/html");

chdir $targdir;

opendir(DIR, "$targdir") or die "can't opendir $targdir: $!";
map unlink, grep { /$filter/ && -f "./$_" } readdir(DIR) 
    or die "can't delete old files: $!";
closedir DIR or die "can't close dir .: $!";
chdir($srcdir);

system($command) == 0 or die "couldn't execute $command from $srcdir";

chdir($targdir);

run_perl_script getcwd(), 
    Cwd::realpath("$ASL_ROOT/adobe_source_libraries/tools/update_offline.pl"), 
    "$targdir",  
    ".*";
