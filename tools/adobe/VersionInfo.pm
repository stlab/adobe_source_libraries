# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2::VersioinInfo - Make Codex 2.0 VersionInfo.xml files.

=head1 SYNOPSIS

    # Import module.
    use Adobe::Codex2::VersionInfo;

    # Create VersionInfo object.
    my $VersionInfo = Adobe::Codex2::VersionInfo->new();

    # Create <build> node.
    $VersionInfo->AddBuild(...);

    # Add <repository> nodes.
    $VersionInfo->AddRepository(...);
    $VersionInfo->AddRepository(...);
    
    # Add items to <components> node.
    $VersionInfo->AddComponent("Component1/VersionInfo.xml");
    $VersionInfo->AddComponent("Component2/VersionInfo.xml");
    
    # Add items to <metadata> node.
    $VersionInfo->AddMetadata(key => "dog", Value => "bark");
    $VersionInfo->AddMetadata(key => "cat", Value => "meow");
    
    # Add items to <fileinfo> node.
    $VersionInfo->AddFileinfo(name => "path/to/file1", md5 => "...");
    $VersionInfo->AddFileinfo(name => "path/to/file2", md5 => "...");

    # Print XML.
    print $VersionInfo->ToString();

    # Save XML.
    $VersionInfo->Save("VersionInfo_NEW.xml");

=head1 DESCRIPTION

Adobe::Codex2::VersionInfo provides functions for creating Codex 2.0
format VersionInfo.xml files.

=head1 LIMITATIONS

Some important notes about how this module works:

* The module does not validate the XML. The only thing the module
checks is that this structure exists in the XML:

    <manifest [version="x"]>
        <versioninfo>
            <build ...>
                ...
            </build>
        </versioninfo>
    </manifest>

And that's it. As long as the XML has the /manifest/versioninfo/build
structure, the module will accept it.

* The DOM that the module creates is not live. That means that if you
make changes to the XML file outside the module while you're using the
module to update the DOM, the DOM won't pick up the external changes
and when you use the "Save" method, the XML file will get overwritten.

* It doesn't check for duplicated items. If you call one of the "Add"
methods (AddRepository, AddComponent, AddMetadata, AddFileinfo) and
add data that already exists, you'll have two items in the final XML.

* It doesn't convert a 1.x format file into 2.0 format. If you read a
1.x file with the "new" method, then update the <build> node with the
"AddBuild" method, you'll end up with an invalid VersionInfo XML file.

Note that it's fine to use 1.x format files with "AddComponents"; you
can have 1.x format <build> nodes inside the <components> node of a
2.0 format file.

=head2 SEE ALSO

    For details about the Codex project:
    https://zerowing.corp.adobe.com/display/codex/Codex+Home

    For details about 2.0 VersionInfo files:
    https://zerowing.corp.adobe.com/display/codex/Codex+2.0+Version+Info+Schema

=head1 VARIABLES/CONFIGURATION

    None.

=head1 DEPENDENCIES

    XML::DOM

=head1 SUBROUTINES/METHODS

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2::VersionInfo;

require 5.8.0;
use strict;
use warnings;

use base qw(Exporter);
our $VERSION     = "0.2.0";
our @ISA         = qw();
our @EXPORT      = qw();
our @EXPORT_OK   = qw();
our %EXPORT_TAGS = ();

# Get Perl modules.
use XML::DOM;

# -------------------------------------------------------------------------
# Module variables.
# -------------------------------------------------------------------------
my $DOM;

# -------------------------------------------------------------------------
=pod

=head2 new

    Create new VersionInfo object.

    Usage:
    my $VersionInfo = Adobe::Codex2::VersionInfo->new($File);

    Input:
    $File = Path to file to use, optional.

    Output:
    success = VersionInfo ojbect.
    failure = die() with error text.

    Notes:
    * If $File is not defined, defaults to "VersionInfo.xml".

=cut
# -------------------------------------------------------------------------
sub new
{
    # Get arguments.
    my $Class = shift;
    my $File  = shift;

    # Check arguments.
    $File = "VersionInfo.xml" unless (defined $File);

    # Set up object.
    my $Self = {};
    bless $Self, ref($Class) || $Class;
    
    # Create XML::DOM parser object.
    my $XMLParser = XML::DOM::Parser->new();
    
    # If $File exists, try to read DOM from it.
    if (-f $File)
    {
        $DOM = eval { $XMLParser->parsefile($File); };
        die("Adobe::Codex2::VersionInfo::new - Error parsing file: $@\n") if ($@);
    }
    else # Create new DOM.
    {
        $DOM = eval { $XMLParser->parse("<manifest version=\"2.0\">\n</manifest>\n"); };
        die("Adobe::Codex2::VersionInfo::new - Error creating DOM: $@\n") if ($@);
    }
    
    # Save object values.
    $Self->{'File'} = $File;

    # Return object.
    return $Self;
}

# -------------------------------------------------------------------------
=pod
    
=head2 DESTROY

    Object destructor.

    Called automatically when object goes out of scope; should not be
    called directly.

=cut
# -------------------------------------------------------------------------
sub DESTROY
{
    # Get arguments.
    my $Self = shift;

    # Clean up.
    $DOM->dispose() if (defined $DOM);

    # Return something.
    return 1;
}

# -------------------------------------------------------------------------
=pod

=head2 AddBuild
    
    Add (or update) <build> element in XML.

    Usage:
    $VersionInfo->AddBuild(build          => "20010101.m.100",
                           compilertarget => "Release",
                           datetime       => "2001/01/01:12:00:00",
                           format         => "RIBS Installer",
                           lang           => "en_GB,en_US,fr_CA,es_MX",
                           licensemodel   => "Retail",
                           platform       => "win32",
                           product        => "InDesign",
                           subproduct     => "Application",
                           version        => "6.0");
    
    Input:
    build          = Build identifier, required.
    compilertarget = Compiler target, optional.
    datetime       = Date/time value in YYYY/MM/DD:hh:mm:ss format, optional.
    format         = Format identifier, optional.
    lang           = Language codes, required.
    licensemodel   = License model identifier, optional.
    platform       = Platform strings, required.
    product        = Product name, required.
    subproduct     = Sub-product name, optional.
    version        = Version string, required.
    
    Output:
    success = 1
    failure = die() with error text.

    Notes:
    * 'datetime' will use current date/time value if not defined.

=cut
# -------------------------------------------------------------------------
sub AddBuild
{
    # Get arguments.
    my $Self = shift;
    my %Args = (build          => undef, # required
                compilertarget => "",    # optional
                datetime       => undef, # optional
                format         => "",    # optional
                lang           => undef, # required
                licensemodel   => "",    # optional
                platform       => undef, # required
                product        => undef, # required
                subproduct     => "",    # optional
                version        => undef, # required
                @_);
    
    # Check arguments.
    die("Adobe::Codex2::VersionInfo::AddBuild - Error: 'build' is not defined.\n")    unless (defined $Args{'build'});
    die("Adobe::Codex2::VersionInfo::AddBuild - Error: 'lang' is not defined.\n")     unless (defined $Args{'lang'});
    die("Adobe::Codex2::VersionInfo::AddBuild - Error: 'platform' is not defined.\n") unless (defined $Args{'platform'});
    die("Adobe::Codex2::VersionInfo::AddBuild - Error: 'product' is not defined.\n")  unless (defined $Args{'product'});
    die("Adobe::Codex2::VersionInfo::AddBuild - Error: 'version' is not defined.\n")  unless (defined $Args{'version'});
    
    # Check datetime value.
    if (defined $Args{'datetime'})
    {
        die("Adobe::Codex2::VersionInfo::AddBuild - Error 'datetime' must be in 'YYYY/MM/DD:hh:mm:ss' format.\n")
            unless ($Args{'datetime'} =~ /^\d\d\d\d\/\d\d\/\d\d:\d\d:\d\d:\d\d$/);
    }
    else # Use current date/time value.
    {
        my ($ss, $mm, $hh, $DD, $MM, $YY, $WD, $YD, $DST) = localtime();
        
        # Set single-digit values to have leading zero.
        $ss = "0" . $ss if ($ss < 10);
        $mm = "0" . $mm if ($mm < 10);
        $hh = "0" . $hh if ($hh < 10);
        $DD = "0" . $DD if ($DD < 10);

        # Month value is returned as a range from 0..11, so add one to get
        # 1..12 range before checking for single-digit value.
        $MM++;
        $MM = "0" . $MM if ($MM < 10);
        
        # Year value needs 1900 added to get current year.
        $YY += 1900;
        
        # Update datetime arg.
        $Args{'datetime'} = "$YY/$MM/$DD:$hh:$mm:$ss";
    }

    # Create <build> node.
    my $BuildNode = _GetBuildNode($DOM);
    $BuildNode->setAttribute("build",          $Args{'build'});
    $BuildNode->setAttribute("compilertarget", $Args{'compilertarget'});
    $BuildNode->setAttribute("datetime",       $Args{'datetime'});
    $BuildNode->setAttribute("format",         $Args{'format'});
    $BuildNode->setAttribute("lang",           $Args{'lang'});
    $BuildNode->setAttribute("licensemodel",   $Args{'licensemodel'});
    $BuildNode->setAttribute("platform",       $Args{'platform'});
    $BuildNode->setAttribute("product",        $Args{'product'});
    $BuildNode->setAttribute("subproduct",     $Args{'subproduct'});
    $BuildNode->setAttribute("version",        $Args{'version'});
    
    # Return something.
    return 1;
}

# -------------------------------------------------------------------------
=pod

=head2 AddMetadata
    
    Add <item> element to <metadata> element.

    Usage:
    $VersionInfo->AddMetadata(key => "dog", Value => "bark");
    $VersionInfo->AddMetadata(key => "cat", Value => "meow");

    Input:
    key   = Value for "key" attribute in <item> element.
    value = Value for "value" attribute in <item> element.
    
    Output:
    success = 1
    failure = die() with error text.

    Notes:
    * Doesn't check for existing <item> elements with same "key".
    * Will escape key/value text, so you can use &/>/< chars.

=cut
# -------------------------------------------------------------------------
sub AddMetadata
{
    # Get arguments.
    my $Self = shift;
    my %Args = (key   => undef, # required
                value => undef, # required
                @_);

    # Check arguments.
    die("Adobe::Codex2::VersionInfo::AddMetadata - Error: 'key' is not defined.\n")   unless (defined $Args{'key'});
    die("Adobe::Codex2::VersionInfo::AddMetadata - Error: 'value' is not defined.\n") unless (defined $Args{'value'});

    # Convert special characters.
    $Args{'key'} =~ s/&/&amp;/g;
    $Args{'key'} =~ s/</&lt;/g;
    $Args{'key'} =~ s/>/&gt;/g;

    $Args{'value'} =~ s/&/&amp;/g;
    $Args{'value'} =~ s/</&lt;/g;
    $Args{'value'} =~ s/>/&gt;/g;

    # Get <build> node.
    my $BuildNode = _GetBuildNode($DOM);

    # Get <metadata> node.
    my $MetadataNode;
    MTEST: foreach my $ChildNode ($BuildNode->getChildNodes())
    {
        if ($ChildNode->getNodeName() eq "metadata")
        {
            $MetadataNode = $ChildNode;
            last MTEST;
        }
    }

    unless (defined $MetadataNode)
    {
        $MetadataNode = $DOM->createElement("metadata");
        $BuildNode->appendChild($DOM->createTextNode("\n"));
        $BuildNode->appendChild($MetadataNode);
        $BuildNode->appendChild($DOM->createTextNode("\n"));
    }
    
    # Create <item> node.
    my $ItemNode = $DOM->createElement("item");
    $ItemNode->setAttribute("key",   $Args{'key'});
    $ItemNode->setAttribute("value", $Args{'value'});
    
    $MetadataNode->appendChild($ItemNode);
    $MetadataNode->appendChild($DOM->createTextNode("\n"));
    
    # Return something.
    return 1;
}

# -------------------------------------------------------------------------
=pod

=head2 AddFileinfo
    
    Add <file> element to <fileinfo> element.

    Usage:
    $VersionInfo->AddFileinfo(name => "path/to/file1", md5 => "...");
    $VersionInfo->AddFileinfo(name => "path/to/file2", md5 => "...");

    Input:
    name = Value for "name" attribute in <file> element.
    md5  = Value for "md5" attribute in <file> element.

    Output:
    success = 1
    failure = die() with error text.

    Notes:
    * Doesn't check for existing <file> elements with same "name".

=cut
# -------------------------------------------------------------------------
sub AddFileinfo
{
    # Get arguments.
    my $Self = shift;
    my %Args = (name => undef, # required
                md5  => undef, # required
                @_);
    
    # Check arguments.
    die("Adobe::Codex2::VersionInfo::AddFileinfo - Error: 'name' is not defined.\n") unless (defined $Args{'name'});
    die("Adobe::Codex2::VersionInfo::AddFileinfo - Error: 'md5' is not defined.\n")  unless (defined $Args{'md5'});

    # Get <build> node.
    my $BuildNode = _GetBuildNode($DOM);

    # Get <fileinfo> node.
    my $FileinfoNode;
    FTEST: foreach my $ChildNode ($BuildNode->getChildNodes())
    {
        if ($ChildNode->getNodeName() eq "fileinfo")
        {
            $FileinfoNode = $ChildNode;
            last FTEST;
        }
    }

    unless (defined $FileinfoNode)
    {
        $FileinfoNode = $DOM->createElement("fileinfo");
        $BuildNode->appendChild($DOM->createTextNode("\n"));
        $BuildNode->appendChild($FileinfoNode);
        $BuildNode->appendChild($DOM->createTextNode("\n"));
    }

    # Chreate <file> node.
    my $FileNode = $DOM->createElement("file");
    $FileNode->setAttribute("name", $Args{'name'});
    $FileNode->setAttribute("md5",  $Args{'md5'});
    
    $FileinfoNode->appendChild($FileNode);
    $FileinfoNode->appendChild($DOM->createTextNode("\n"));

    # Return something.
    return 1;
}

# -------------------------------------------------------------------------
=pod

=head2 AddRepository
    
    Adds <repository> element to <build> element.

    Usage:
    $VersionInfo->AddRepository(authority => "perforce:1666",
                                path      => "//depot/main/...",
                                query     => "\@10101",
                                scheme    => "p4");
    
    Input:
    authority = FQDN (+ port) for server.
    path      = Path to resource on server.
    query     = Condition for path, optional.
    scheme    = Scheme for server.

    Output:
    success = 1
    failure = die() with error text.

    Notes:
    * 'scheme' is usually something like p4, smb, ftp, http, etc.

=cut
# -------------------------------------------------------------------------
sub AddRepository
{
    # Get arguments.
    my $Self = shift;
    my %Args = (authority => undef, # required
                path      => undef, # required
                query     => "",    # optional
                scheme    => undef, # required
                @_);

    # Check arguments.
    die("Adobe::Codex2::VersionInfo::AddRepository - Error: 'authority' is not defined.\n") unless (defined $Args{'authority'});
    die("Adobe::Codex2::VersionInfo::AddRepository - Error: 'path' is not defined.\n")      unless (defined $Args{'path'});
    die("Adobe::Codex2::VersionInfo::AddRepository - Error: 'scheme' is not defined.\n")    unless (defined $Args{'scheme'});

    # Get <build> node.
    my $BuildNode = _GetBuildNode($DOM);
    
    # Create <repository> node.
    my $RepNode = $DOM->createElement("repository");
    $RepNode->setAttribute("authority", $Args{'authority'});
    $RepNode->setAttribute("path",      $Args{'path'});
    $RepNode->setAttribute("query",     $Args{'query'});
    $RepNode->setAttribute("scheme",    $Args{'scheme'});

    $BuildNode->appendChild($RepNode);
    $BuildNode->appendChild($DOM->createTextNode("\n"));
    
    # Return something.
    return 1;
}

# -------------------------------------------------------------------------
=pod

=head2 AddComponent
    
    Add <build> element to <components> element.

    Usage:
    $VersionInfo->AddComponent($File);

    Input:
    $File = Path to component VersionInfo.xml file.

    Output:
    success = 1;
    failure = die() with error text.

    Notes:
    none

=cut
# -------------------------------------------------------------------------
sub AddComponent
{
    # Get arguments.
    my $Self = shift;
    my $File = shift;

    # Check arguments.
    die("Adobe::Codex2::VersionInfo::AddComponent - Error: 'File' is not defined.\n")  unless (defined $File);
    die("Adobe::Codex2::VersionInfo::AddComponent - Error: '$File' does not exist.\n") unless (-f $File);

    # Create XML::DOM parser object.
    my $XMLParser = XML::DOM::Parser->new();
    
    # Try to read DOM from file.
    my $CompDOM = eval { $XMLParser->parsefile($File); };
    die("Adobe::Codex2::VersionInfo::AddComponent - Error parsing file: $@\n") if ($@);
    
    # Get <build> node from file.
    my $CompBuildNode = _GetBuildNode($CompDOM);
    
    # Get <build> node.
    my $BuildNode = _GetBuildNode($DOM);
    
    # Get <components> node.
    my $ComponentsNode;
    CTEST: foreach my $ChildNode ($BuildNode->getChildNodes())
    {
        if ($ChildNode->getNodeName() eq "components")
        {
            $ComponentsNode = $ChildNode;
            last CTEST;
        }
    }

    unless (defined $ComponentsNode)
    {
        $ComponentsNode = $DOM->createElement("components");
        $BuildNode->appendChild($ComponentsNode);
        $BuildNode->appendChild($DOM->createTextNode("\n"));
    }
    
    # Add file <build> node to <components> node.
    $CompBuildNode->setOwnerDocument($DOM);
    $ComponentsNode->appendChild($DOM->createTextNode("\n"));
    $ComponentsNode->appendChild($CompBuildNode);
    $ComponentsNode->appendChild($DOM->createTextNode("\n"));

    # Return something.
    return 1;
}

# -------------------------------------------------------------------------
=pod

=head2 ToString

    Output XML as string.

    Usage:
    print $VersionInfo->ToString();

    Input:
    none

    Output:
    success = String representation of XML.
    failure = die() with error text.

    Notes:
    none

=cut
# -------------------------------------------------------------------------
sub ToString
{
    # Get arguments.
    my $Self = shift;
    
    # Check arguments.
    # Nothing to check.

    # Return string of XML.
    return $DOM->toString();
}

# -------------------------------------------------------------------------
=pod

=head2 Save
    
    Save XML to file.

    Usage:
    $VersionInfo->Save($File);
    
    Input:
    $File = File to save to, optional.

    Output:
    success = 1
    failure = die() with error text.

    Notes:
    * If $File is not defined, uses file set up during "new" method.

=cut
# -------------------------------------------------------------------------
sub Save
{
    # Get arguments.
    my $Self = shift;
    my $File = shift;

    # Ceck arguments.
    $File = $Self->{'File'} unless (defined $File);

    # Make sure file is unlocked.
    chmod(0666, $File) if (-f $File);

    # Save the file.
    $DOM->printToFile($File);

    # Return something.
    return 1;
}

# -------------------------------------------------------------------------
=pod

=head2 _GetBuildNode
    
    Utility routine to get <build> node from DOM.

    Not intended to be called directly.

=cut
# TODO Should probably verify that root node is <manifest>.
# -------------------------------------------------------------------------
sub _GetBuildNode
{
    # Get arguments.
    my $NewDOM = shift;

    # Storage for <build> node.
    my $BuildNode;
    
    # Get first <versioninfo> node.
    my $RootNode = $NewDOM->getDocumentElement();
    my $VersionInfoNode;
    VTEST: foreach my $ChildNode ($RootNode->getChildNodes())
    {   
        if ($ChildNode->getNodeName() eq "versioninfo")
        {
            $VersionInfoNode = $ChildNode;
            last VTEST;
        }
    }
    
    if (defined $VersionInfoNode)
    {
        BTEST: foreach my $ChildNode ($VersionInfoNode->getChildNodes())
        {
            if ($ChildNode->getNodeName() eq "build")
            {
                $BuildNode = $ChildNode;
                last BTEST;
            }
        }
    }
    else # Make new <versioninfo> node.
    {
        $VersionInfoNode = $NewDOM->createElement("versioninfo");
        $RootNode->appendChild($VersionInfoNode);
        $RootNode->appendChild($NewDOM->createTextNode("\n"));
    }

    unless (defined $BuildNode)
    {
        $BuildNode = $NewDOM->createElement("build");
        $VersionInfoNode->appendChild($BuildNode);
        $VersionInfoNode->appendChild($NewDOM->createTextNode("\n"));
    }
    
    # Return <build> node.
    return $BuildNode;
}

1;
__END__

=pod

=head1 AUTHOR

    Dave Foglesong (fogleson@adobe.com)

=head1 COPYRIGHT

    Copyright 2008 Adobe Systems Incorporated. All rights reserved.

=cut


