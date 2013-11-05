# =========================================================================
# Adobe::ShellUtils.pm
# Perl module for common shell tasks.
# =========================================================================

# Module setup.
package Adobe::ShellUtils;

use strict;
use warnings;

use base qw(Exporter);
our $VERSION   = "1.00";
our @ISA       = qw(Exporter);
our @EXPORT    = qw();
our @EXPORT_OK = qw(cmd_in_dir zip tar copy_dir make_writeable
 mk_dir build force_rmdir build_msvc run_perl_script p4_edit
 p4_revert_unchanged doxygen);

# Get other modules.
use XML::Simple;
use Carp;

#!/usr/bin/perl -w

use strict;
use warnings;

use Cwd;

### subroutines

sub cmd_in_dir($$@) {
    my ($cmd, $options, $src_parent_dir, @args) = @_;
    print "executing ${cmd} $options @args in $src_parent_dir\n";
    my $wd = getcwd();
    chdir($src_parent_dir) or die "Couldn't set working directory to $src_parent_dir: $!";;
    my @cmd_and_args=($cmd, $options, @args);
    my $cmd_dir=getcwd();
    system(@cmd_and_args) == 0 or die "Couldn't execute ${cmd} $options @args in $cmd_dir $!";
    chdir($wd) or die "Couldn't restore working directory to $wd: $!";
}

sub zip ($$$) {
    cmd_in_dir "zip", "-Xrq9", @_;
}

sub tar ($$$) {
    cmd_in_dir "tar", "-czf", @_;
}

#sub copy_dir ($$) {
#    cmd_in_dir "cp", "-prT", getcwd(), @_;
#}

sub copy_dir ($$) {
    cmd_in_dir "cp", "-pr", getcwd(), @_;
}

sub make_writeable($) {
    cmd_in_dir "chmod", "-R", getcwd(), "a+rw", @_;
}

sub mk_dir($) {
    cmd_in_dir "mkdir", "-p", getcwd(), @_;
}
sub force_rmdir($) {
    cmd_in_dir "rm", "-r", getcwd(), @_;
}
sub build($@) {
    cmd_in_dir "bjam", "-j2", @_;
}
sub build_msvc($$$$) {
    cmd_in_dir "devenv", "", @_;
}

sub run_perl_script(@) {
    cmd_in_dir "perl", "-w", @_;
}

sub p4_edit($@) {
    cmd_in_dir "p4", "edit", shift, @_;
}

sub p4_revert_unchanged($@) {
    cmd_in_dir "p4", "revert", shift, "-a",  @_;
}

sub p4_add($@) {
    cmd_in_dir "p4", "add", shift, @_;
}

#if 0
sub p4_where($$) {
    my $wd = getcwd();
    my $cmd_dir = shift;
    my $rel_path = shift;
    chdir($cmd_dir) or die "Couldn't set working directory to $cmd_dir: $!";;
    print "executing p4 where $rel_path in $cmd_dir\n";
    open(WHERE, "p4 where $rel_path | ") or die "can't open p4 where $rel_path : $!";
    my @result;
    while (<WHERE>) { @result = split;}               # do something with input
    close(WHERE)                      || die "can't close p4 where $rel_path : $!";
    chdir($wd) or die "Couldn't restore working directory to $wd: $!";
    return shift @result; 
}
#endif

sub doxygen($@) {
    my $dir=shift;
    my $dox_config=shift;
    cmd_in_dir "doxygen", $dox_config, $dir;
}




1;
