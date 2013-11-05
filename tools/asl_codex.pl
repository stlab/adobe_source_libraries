#!/usr/bin/perl

use warnings ;
use strict ;
use FindBin qw($Bin);

use Adobe::Codex2::VersionInfo ;

die "usage: asl_codex build-number" unless @ARGV == 1;

my ($build_number)=@ARGV;

# Create VersionInfo object.

my $VersionInfo = Adobe::Codex2::VersionInfo->new(File => "VersionInfo.xml");

# Add build element.
$VersionInfo->AddBuild(
                       build      => $build_number,
                       lang       => "mul",
                       platform   => "source",      
                       product    => "Adobe Source Library",  
                       version    => "1");

# Add server element.
$VersionInfo->AddRepository(scheme    => "p4",
                        authority => "boxer.corp.adobe.com:1770",
                        path      => "//adobe_source_libraries/...",
                        query     => "See https://zerowing.corp.adobe.com/display/stlab/Changelist+Numbers+For+ASL+Release for changelist info");                #change this

# Add component elements.
my @ComponentFiles = ("$Bin/../../boost_libraries/adobe_information/VersionInfo.xml", "$Bin/../../intel_tbb_libraries/adobe_information/VersionInfo.xml");
foreach my $ComponentFile (@ComponentFiles)
{
    $VersionInfo->AddComponent($ComponentFile);
}

# Save file.
$VersionInfo->Save();

