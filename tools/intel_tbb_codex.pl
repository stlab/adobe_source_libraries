#!/usr/bin/perl

use warnings ;
use strict ;
use FindBin;

use Adobe::VersionInfo;

die "usage: intel_tbb_codex build-number" unless @ARGV == 1;

my ($build_number)=@ARGV;

# Create VersionInfo object.

my $VersionInfo = Adobe::VersionInfo->new(File => "VersionInfo.xml");

# Add build element.
$VersionInfo->AddBuild(
                       build      => $build_number,
                       lang       => "mul",
                       platform   => "source",      
                       product    => "Intel Thread Building Blocks",  
                       version    => "tbb21_009oss");


# Add server element.
$VersionInfo->AddRepository(scheme    => "p4",
                        authority => "boxer.corp.adobe.com:1770",
                        path      => "//intel_tbb_libraries/...",
                        query     => "See https://zerowing.corp.adobe.com/display/stlab/Changelist+Numbers+For+ASL+Release for actual changelist and version info");

# Save file.
$VersionInfo->Save();

