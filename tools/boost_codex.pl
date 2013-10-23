#!/usr/bin/perl

use warnings ;
use strict ;
use FindBin;

use Adobe::Codex2::VersionInfo;

die "usage: boost_codex build-number" unless @ARGV == 1;

my ($build_number)=@ARGV;

# Create VersionInfo object.

my $VersionInfo = Adobe::Codex2::VersionInfo->new(File => "VersionInfo.xml");

# Add build element.
$VersionInfo->AddBuild(
                       build     => $build_number,
                       lang       => "mul",
                       platform   => "source",      
                       product    => "Boost",  
                       version    => "1_36_0");

# Add server element.
$VersionInfo->AddRepository(scheme    => "p4",
                        authority => "boxer.corp.adobe.com:1770",
                        path      => "//boost_libraries/...",
                        query     => "See https://zerowing.corp.adobe.com/display/stlab/Changelist+Numbers+For+ASL+Release for changelist info");            

# Save file.
$VersionInfo->Save();

