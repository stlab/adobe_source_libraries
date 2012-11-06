#!/usr/bin/perl

use warnings;
use strict;

use File::Find;

## following offline recipe from
## http://kb.perforce.com/UserTasks/WorkingDisconnected

die "usage: update_offline target-dir file-pattern" unless @ARGV == 2;

my ($targdir, $filter)=@ARGV;

chdir($targdir);

#delete files that no longer exist on the client from the depot
my $command = "p4 diff -sd './...' | p4 -x - delete";
print "deleting files that no longer exist on the client from the depot\n";
print "executing $command\n";
system($command) == 0 
    or die "couldn't $command from $targdir";

#edit files that have changed on the client in the depot
$command = "p4 diff -se './...' | p4 -x - edit";
print "editing files that have changed on the client in the depot\n";
print "executing $command\n";
system($command) == 0 
    or die "couldn't $command from $targdir";

#add files that are new matching filter on the client to the depot (re-adds harmless)
print "add files that are new matching filter on the client to the depot (failed re-adds harmless)";
    print "piping files matching /$filter/ in $targdir to p4 -x - add";
open(ADD, "| p4 -x - add") or die "couldn't open pipe to p4 add in $targdir: $!";
sub add_file {
    return unless /$filter/ && -f;
    print ADD "${File::Find::name}\n";
}

find(\&add_file, ".");

close ADD or die "couldn't p4 add files in $targdir: $!";

print "succeeded, ok to submit\n";

