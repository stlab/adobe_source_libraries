# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2::Build - Perl interface to Codex 2.0 system.

=head1 DESCRIPTION

See POD from Adobe::Codex2 for details on using this module.

=head1 SUBROUTINES/METHODS

    AddBuild
    AddNote
    DeleteBuild
    GetAllBuildsFromProfile
    GetBuildByID
    GetBuildComponents
    GetBuildHistory
    GetBuilds
    GetBuildsByLocation
    GetBuildFromProfile
    GetBuildsFromProfile
    GetBuildsFromProfileWithComponent
    GetBuildsUsing
    SetCertLevel
    SetMetadata
    SetStatus
    SetLocation

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2::Build;

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
# None.

# Get Codex modules.
use Adobe::Codex2::SOAP;
use Adobe::Codex2::Types::build;
use Adobe::Codex2::Types::buildlist;
use Adobe::Codex2::Types::id;
use Adobe::Codex2::Types::operationresult;

# -------------------------------------------------------------------------
=pod

=head2 AddBuild
    
    Usage:
    my $id = $Codex->AddBuild(manifestfile    => $ManifestFile,
                              uri             => $URI,
                              certlevel       => $CertLevel,
                              statusname      => $StatusName,
                              ldapcredentials => {userid => $UserID, password => $Password});

    Input:
    ...

    Output:
    ...

  <xs:element name="AddBuildRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:manifestfile"/>
        <xs:element ref="schemas:uri"/>
        <xs:element ref="schemas:certlevel"/>
        <xs:element ref="schemas:statusname"/>
        <xs:element ref="schemas:ldapcredentials"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="AddBuildResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub AddBuild
{
    # Get arguments.
    my $Self = shift;
    my %Args = (manifestfile    => undef,
                uri             => undef,
                certlevel       => undef,
                statusname      => undef,
                ldapcredentials => {userid   => undef,
                                    password => undef},
                @_);

    # Check arguments.
    die("Adobe::Codex2::AddBuild - Error: 'manifestfile' argument is not defined.\n")  unless (defined $Args{'manifestfile'});
    die("Adobe::Codex2::AddBuild - Error: 'uri' argument is not defined.\n")           unless (defined $Args{'uri'});
    die("Adobe::Codex2::AddBuild - Error: 'certlevel' argument is not defined.\n")     unless (defined $Args{'certlevel'});
    die("Adobe::Codex2::AddBuild - Error: 'statusname' argument is not defined.\n")    unless (defined $Args{'statusname'});
    die("Adobe::Codex2::AddBuild - Error: LDAP 'userid' argument is not defined.\n")   unless (defined $Args{'ldapcredentials'}->{'userid'});
    die("Adobe::Codex2::AddBuild - Error: LDAP 'password' argument is not defined.\n") unless (defined $Args{'ldapcredentials'}->{'password'});
    
    # Load the manifest file.
    die("Adobe::Codex2::AddBuild - Error: Manifest file does not exist.\n") unless (-f $Args{'manifestfile'});
    my $ManifestData;
    open(MANIFEST, $Args{'manifestfile'}) or die("Adobe::Codex2::AddBuild - Error: Could not open manifest file: $!\n");
    while (<MANIFEST>)
    {
        $ManifestData .= $_;
    }
    close(MANIFEST);
    
    # Escape special characters.
    $ManifestData =~ s/&/&amp;/g;
    $ManifestData =~ s/</&lt;/g;
    $ManifestData =~ s/>/&gt;/g;

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:AddBuildRequest>
            <c2pl:manifestfile>' . $ManifestData . '</c2pl:manifestfile>
            <c2pl:uri>' . $Args{'uri'} . '</c2pl:uri>
            <c2pl:certlevel>' . $Args{'certlevel'} . '</c2pl:certlevel>
            <c2pl:statusname>' . $Args{'statusname'} . '</c2pl:statusname>
            <c2pl:ldapcredentials>
                <c2pl:userid>' . $Args{'ldapcredentials'}->{'userid'} . '</c2pl:userid>
                <c2pl:password>' . $Args{'ldapcredentials'}->{'password'} . '</c2pl:password>
            </c2pl:ldapcredentials>
        </c2pl:AddBuildRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "AddBuildResponse", "id");
    return Adobe::Codex2::Types::id::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 AddNote
    
    Usage:
    my %build = $Codex->AddNote(buildid         => $BuildID,
                                notetext        => $NoteText,
                                ldapcredentials => {userid => $UserID, password => $Password});

    Input:
    ...

    Output:
    ...

  <xs:element name="AddNoteRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:buildid"/>
        <xs:element ref="schemas:notetext"/>
        <xs:element ref="schemas:ldapcredentials"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="AddNoteResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:build"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub AddNote
{
    # Get arguments.
    my $Self = shift;
    my %Args = (buildid         => undef,
                notetext        => undef,
                ldapcredentials => {userid   => undef,
                                    password => undef},
                @_);

    # Check arguments.
    die("Adobe::Codex2::AddNote - Error: 'buildid' argument is not defined.\n")       unless (defined $Args{'buildid'});
    die("Adobe::Codex2::AddNote - Error: 'notetext' argument is not defined.\n")      unless (defined $Args{'notetext'});
    die("Adobe::Codex2::AddNote - Error: LDAP 'userid' argument is not defined.\n")   unless (defined $Args{'ldapcredentials'}->{'userid'});
    die("Adobe::Codex2::AddNote - Error: LDAP 'password' argument is not defined.\n") unless (defined $Args{'ldapcredentials'}->{'password'});

    # Escape special characters.
    $Args{'notetext'} =~ s/&/&amp;/g;
    $Args{'notetext'} =~ s/</&lt;/g;
    $Args{'notetext'} =~ s/>/&gt;/g;

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:AddNoteRequest>
            <c2pl:buildid>' . $Args{'buildid'} . '</c2pl:buildid>
            <c2pl:notetext>' . $Args{'notetext'} . '</c2pl:notetext>
            <c2pl:ldapcredentials>
                <c2pl:userid>' . $Args{'ldapcredentials'}->{'userid'} . '</c2pl:userid>
                <c2pl:password>' . $Args{'ldapcredentials'}->{'password'} . '</c2pl:password>
            </c2pl:ldapcredentials>
        </c2pl:AddNoteRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "AddNoteResponse", "build");
    return Adobe::Codex2::Types::build::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 DeleteBuild

    Usage:
    my $result = $Codex->DeleteBuild(id              => $ID,
                                     ldapcredentials => {userid => $UserID, password => $Password});

    Input:
    ...

    Output:
    ...

  <xs:element name="DeleteBuildRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
        <xs:element ref="schemas:ldapcredentials"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="DeleteBuildResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:operationresult"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub DeleteBuild
{
    # Get arguments.
    my %Args = (id              => undef,
                ldapcredentials => {userid   => undef,
                                    password => undef},
                @_);

    # Check arguments.
    die("Adobe::Codex2::DeleteBuild - Error: 'id' argument is not defined.\n")            unless (defined $Args{'id'});
    die("Adobe::Codex2::DeleteBuild - Error: LDAP 'userid' argument is not defined.\n")   unless (defined $Args{'ldapcredentials'}->{'userid'});
    die("Adobe::Codex2::DeleteBuild - Error: LDAP 'password' argument is not defined.\n") unless (defined $Args{'ldapcredentials'}->{'password'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:DeleteBuildRequest>
            <c2pl:id>' . $Args{'id'} . '</c2pl:id>
            <c2pl:ldapcredentials>
                <c2pl:userid>' . $Args{'ldapcredentials'}->{'userid'} . '</c2pl:userid>
                <c2pl:password>' . $Args{'ldapcredentials'}->{'password'} . '</c2pl:password>
            </c2pl:ldapcredentials>
        </c2pl:DeleteBuildRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "DeleteBuildResponse", "operationresult");
    return Adobe::Codex2::Types::operationresult::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 GetAllBuildsFromProfile

    Usage:
    my @buildlist = $Codex->GetAllBuildsFromProfile(productname  => $ProductName,
                                                    versionname  => $VersionName,
                                                    profilename  => $ProfileName,
                                                    platformname => $PlatformName,
                                                    languagecode => $LanguageCode);
    
    Input:
    ...

    Output:
    ...

  <xs:element name="GetAllBuildsFromProfileRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:productname"/>
        <xs:element ref="schemas:versionname"/>
        <xs:element ref="schemas:profilename"/>
        <xs:element ref="schemas:platformname"/>
        <xs:element ref="schemas:languagecode"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="GetAllBuildsFromProfileResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:buildlist"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub GetAllBuildsFromProfile
{
    # Get arguments.
    my $Self = shift;
    my %Args = (productname  => undef, 
                versionname  => undef,
                profilename  => undef,
                platformname => undef,
                languagecode => undef,
                @_);

    # Check arguments.
    die("Adobe::Codex2::GetAllBuildsFromProfile - Error: 'productname' argument is not defined.\n")  unless (defined $Args{'productname'});
    die("Adobe::Codex2::GetAllBuildsFromProfile - Error: 'versionname' argument is not defined.\n")  unless (defined $Args{'versionname'});
    die("Adobe::Codex2::GetAllBuildsFromProfile - Error: 'profilename' argument is not defined.\n")  unless (defined $Args{'profilename'});
    die("Adobe::Codex2::GetAllBuildsFromProfile - Error: 'platformname' argument is not defined.\n") unless (defined $Args{'platformname'});
    die("Adobe::Codex2::GetAllBuildsFromProfile - Error: 'languagecode' argument is not defined.\n") unless (defined $Args{'languagecode'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetAllBuildsFromProfileRequest>
            <c2pl:productname>' . $Args{'productname'} . '</c2pl:productname>
            <c2pl:versionname>' . $Args{'versionname'} . '</c2pl:versionname>
            <c2pl:profilename>' . $Args{'profilename'} . '</c2pl:profilename>
            <c2pl:platformname>' . $Args{'platformname'} . '</c2pl:platformname>
            <c2pl:languagecode>' . $Args{'languagecode'} . '</c2pl:languagecode>
        </c2pl:GetAllBuildsFromProfileRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetAllBuildsFromProfileResponse", "buildlist");
    return Adobe::Codex2::Types::buildlist::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 GetBuildById

    Usage:
    my %build = $Codex->GetBuildById(buildid => $BuildID);

    Input:
    ...

    Output:
    ...

  <xs:element name="GetBuildByIdRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:buildid"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="GetBuildByIdResponse">
    <xs:complexType>
      <xs:sequence>
        <xs:element ref="schemas:build"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub GetBuildById
{
    # Get arguments.
    my $Self = shift;
    my %Args = (buildid => undef,
                @_);

    # Check arguments.
    die("Adobe::Codex2::GetBuildById - Error: 'buildid' argument is not defined.\n") unless (defined $Args{'buildid'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetBuildByIdRequest>
            <c2pl:buildid>' . $Args{'buildid'} . '</c2pl:buildid>
        </c2pl:GetBuildByIdRequest>');
        
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetBuildByIdResponse", "build");
    return Adobe::Codex2::Types::build::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 GetBuildComponents
    
    Usage:
    my @buildlist = $Codex->GetBuildComponents(id => $ID);

    Input:
    ...

    Output:
    ...

  <xs:element name="GetBuildComponentsRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="GetBuildComponentsResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:buildlist"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub GetBuildComponents
{
    # Get arguments.
    my $Self = shift;
    my %Args = (id => undef,
                @_);

    # Check arguments.
    die("Adobe::Codex2::GetBuildComponents - Error: 'id' argument is not defined.\n") unless (defined $Args{'id'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetBuildComponentsRequest>
            <c2pl:id>' . $Args{'id'} . '</c2pl:id>
        </c2pl:GetBuildComponentsRequest>');
        
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetBuildComponentsResponse", "buildlist");
    return Adobe::Codex2::Types::buildlist::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 GetBuildHistory
    
    Usage:
    my @buildlist = $Codex->GetBuildHistory(id => $ID);

    Input:
    ...

    Output:
    ...

  <xs:element name="GetBuildHistoryRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  
  <xs:element name="GetBuildHistoryResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:buildlist"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>  

=cut
# -------------------------------------------------------------------------
sub GetBuildHistory
{
    # Get arguments.
    my $Self = shift;
    my %Args = (id => undef,
                @_);

    # Check arguments.
    die("Adobe::Codex2::GetBuildHistory - Error: 'id' argument is not defined.\n") unless (defined $Args{'id'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetBuildHistoryRequest>
            <c2pl:id>' . $Args{'id'} . '</c2pl:id>
        </c2pl:GetBuildHistoryRequest>');
        
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetBuildHistoryResponse", "buildlist");
    return Adobe::Codex2::Types::buildlist::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod
    
=head2 GetBuilds
    
    Usage:
    my @BuildList = $Codex->GetBuilds(productname        => $ProductName,
                                      versionname        => $VersionName,
                                      subproductname     => $SubProducNamet,
                                      buildname          => $BuildName,
                                      platformname       => $PlatformName,
                                      languagecode       => $LanguageCode,
                                      compilertargetname => $CompilerTargetName,
                                      licensemodelname   => $LicenseModelName,
                                      formatname         => $FormatName,
                                      certlevel          => $CertLevel,
                                      statusname         => $StatusName,
                                      date               => $Date,
                                      keyvalpair         => {key => val});

    Input:
    ...

    Output:
    ...
    
    TODO Should key/value pairs be a hash ref, or an array ref of "key=value" strings?

  <xs:element name="GetBuildsRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:productname"/>
        <xs:element ref="schemas:versionname"/>
        <xs:element ref="schemas:subproductname"/>
        <xs:element ref="schemas:buildname"/>
        <xs:element ref="schemas:platformname"/>
        <xs:element ref="schemas:languagecode"/>
        <xs:element ref="schemas:compilertargetname"/>
        <xs:element ref="schemas:licensemodelname"/>
        <xs:element ref="schemas:formatname"/>
        <xs:element ref="schemas:certlevel"/>
        <xs:element ref="schemas:statusname"/>
        <xs:element ref="schemas:date"/>
        <xs:element minOccurs="0" name="keyvalpair" type="schemas:keyvalpair"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="GetBuildsResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:buildlist"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub GetBuilds
{
    # Get arguments.
    my $Self = shift;
    my %Args = (productname        => undef,
                versionname        => undef,
                subproductname     => undef,
                buildname          => undef,
                platformname       => undef,
                languagecode       => undef,
                compilertargetname => undef,
                licensemodelname   => undef,
                formatname         => undef,
                certlevel          => undef,
                statusname         => undef,
                date               => undef,
                keyvalpair         => {},
                @_);

    # Check arguments.
    die("Adobe::Codex2::GetBuilds - Error: 'productname' argument is not defined.\n")        unless (defined $Args{'productname'});
    die("Adobe::Codex2::GetBuilds - Error: 'versionname' argument is not defined.\n")        unless (defined $Args{'versionname'});
    die("Adobe::Codex2::GetBuilds - Error: 'subproductname' argument is not defined.\n")     unless (defined $Args{'subproductname'});
    die("Adobe::Codex2::GetBuilds - Error: 'buildname' argument is not defined.\n")          unless (defined $Args{'buildname'});
    die("Adobe::Codex2::GetBuilds - Error: 'platformname' argument is not defined.\n")       unless (defined $Args{'platformname'});
    die("Adobe::Codex2::GetBuilds - Error: 'languagecode' argument is not defined.\n")       unless (defined $Args{'languagecode'});
    die("Adobe::Codex2::GetBuilds - Error: 'compilertargetname' argument is not defined.\n") unless (defined $Args{'compilertargetname'});
    die("Adobe::Codex2::GetBuilds - Error: 'licensemodelname' argument is not defined.\n")   unless (defined $Args{'licensemodelname'});
    die("Adobe::Codex2::GetBuilds - Error: 'formatname' argument is not defined.\n")         unless (defined $Args{'formatname'});
    die("Adobe::Codex2::GetBuilds - Error: 'certlevel' argument is not defined.\n")          unless (defined $Args{'certlevel'});
    die("Adobe::Codex2::GetBuilds - Error: 'statusname' argument is not defined.\n")         unless (defined $Args{'statusname'});
    die("Adobe::Codex2::GetBuilds - Error: 'date' argument is not defined.\n")               unless (defined $Args{'date'});

    # Pull out key/value pair data.
    my $KeyValNodes = "\n";
    foreach my $Key (keys %{$Args{'keyvalpair'}})
    {
        my $Value = $Args{'keyvalpair'}->{$Key};

        # Escape special characters.
        $Key =~ s/&/&amp;/g;
        $Key =~ s/</&lt;/g;
        $Key =~ s/>/&gt;/g;

        $Value =~ s/&/&amp;/g;
        $Value =~ s/</&lt;/g;
        $Value =~ s/>/&gt;/g;

        $KeyValNodes .= "<c2pl:keyvalpair>$Key=$Value</c2pl:keyvalpair>\n";
    }

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetBuildsRequest>
            <c2pl:productname>' . $Args{'productname'} . '</c2pl:productname>
            <c2pl:versionname>' . $Args{'versionname'} . '</c2pl:versionname>
            <c2pl:subproductname>' . $Args{'subproductname'} . '</c2pl:subproductname>
            <c2pl:buildname>' . $Args{'buildname'} . '</c2pl:buildname>
            <c2pl:platformname>' . $Args{'platformname'} . '</c2pl:platformname>
            <c2pl:languagecode>' . $Args{'languagecode'} . '</c2pl:languagecode>
            <c2pl:compilertargetname>' . $Args{'compilertargetname'} . '</c2pl:compilertargetname>
            <c2pl:licensemodelname>' . $Args{'licensemodelname'} . '</c2pl:licensemodelname>
            <c2pl:formatname>' . $Args{'formatname'} . '</c2pl:formatname>
            <c2pl:certlevel>' . $Args{'certlevel'} . '</c2pl:certlevel>
            <c2pl:statusname>' . $Args{'statusname'} . '</c2pl:statusname>
            <c2pl:date>' . $Args{'date'} . '</c2pl:date>' .
            $KeyValNodes .
        '</c2pl:GetBuildsRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetBuildsResponse", "buildlist");
    return Adobe::Codex2::Types::buildlist::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 GetBuildsByLocation

    Usage:
    my @buildlist = $Codex->GetBuildsByLocation(protocol   => $Protocol,
                                                servername => $ServerName,
                                                path       => $Path,
                                                query      => $Query);
    
    Input:
    ...

    Output:
    ...

  <xs:element name="GetBuildsByLocationRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:protocol"/>
        <xs:element ref="schemas:servername"/>
        <xs:element ref="schemas:path"/>
        <xs:element ref="schemas:query"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="GetBuildsByLocationResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:buildlist"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub GetBuildsByLocation
{
    # Get arguments.
    my $Self = shift;
    my %Args = (protocol   => undef,
                servername => undef,
                path       => undef,
                query      => undef,
                @_);

    # Check arguments.
    die("Adobe::Codex2::GetBuildsByLocation - Error: 'protocol' argument is not defined.\n")   unless (defined $Args{'protocol'});
    die("Adobe::Codex2::GetBuildsByLocation - Error: 'servername' argument is not defined.\n") unless (defined $Args{'servername'});
    die("Adobe::Codex2::GetBuildsByLocation - Error: 'path' argument is not defined.\n")       unless (defined $Args{'path'});
    die("Adobe::Codex2::GetBuildsByLocation - Error: 'query' argument is not defined.\n")      unless (defined $Args{'query'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetBuildsByLocationRequest>
            <c2pl:protocol>' . $Args{'protocol'} . '</c2pl:protocol>
            <c2pl:servername>' . $Args{'servername'} . '</c2pl:servername>
            <c2pl:path>' . $Args{'path'} . '</c2pl:path>
            <c2pl:query>' . $Args{'query'} . '</c2pl:query>
        </c2pl:GetBuildsByLocationRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetBuildsByLocationResponse", "buildlist");
    return Adobe::Codex2::Types::buildlist::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 GetBuildFromProfile
    
    Usage:
    my %build = $Codex->GetBuildFromProfile(productname    => $ProductName,
                                            versionname    => $VersionName,
                                            profilename    => $ProfileName,
                                            component      => $Component,
                                            subproductname => $SubProductName,
                                            platformname   => $PlatformName,
                                            languagecode   => $LanguageCode);
    
    Input:
    ...

    Output:
    ...

  <xs:element name="GetBuildFromProfileRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:productname"/>
        <xs:element ref="schemas:versionname"/>
        <xs:element ref="schemas:profilename"/>
        <xs:element ref="schemas:component"/>
        <xs:element ref="schemas:subproductname"/>
        <xs:element ref="schemas:platformname"/>
        <xs:element ref="schemas:languagecode"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="GetBuildFromProfileResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:build"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  
=cut
# -------------------------------------------------------------------------
sub GetBuildFromProfile
{
    # Get arguments.
    my $Self = shift;
    my %Args = (productname    => undef,
                versionname    => undef,
                profilename    => undef,
                component      => undef,
                subproductname => undef,
                platformname   => undef,
                languagecode   => undef,
                @_);

    # Check arguments.
    die("Adobe::Codex2::GetBuildFromProfile - Error: 'productname' argument is not defined.\n")    unless (defined $Args{'productname'});
    die("Adobe::Codex2::GetBuildFromProfile - Error: 'versionname' argument is not defined.\n")    unless (defined $Args{'versionname'});
    die("Adobe::Codex2::GetBuildFromProfile - Error: 'profilename' argument is not defined.\n")    unless (defined $Args{'profilename'});
    die("Adobe::Codex2::GetBuildFromProfile - Error: 'component' argument is not defined.\n")      unless (defined $Args{'component'});
    die("Adobe::Codex2::GetBuildFromProfile - Error: 'subproductname' argument is not defined.\n") unless (defined $Args{'subproductname'});
    die("Adobe::Codex2::GetBuildFromProfile - Error: 'platformname' argument is not defined.\n")   unless (defined $Args{'platformname'});
    die("Adobe::Codex2::GetBuildFromProfile - Error: 'languagecode' argument is not defined.\n")   unless (defined $Args{'languagecode'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetBuildFromProfileRequest>
            <c2pl:productname>' . $Args{'productname'} . '</c2pl:productname>
            <c2pl:versionname>' . $Args{'versionname'} . '</c2pl:versionname>
            <c2pl:profilename>' . $Args{'profilename'} . '</c2pl:profilename>
            <c2pl:component>' . $Args{'component'} . '</c2pl:component>
            <c2pl:subproductname>' . $Args{'subproductname'} . '</c2pl:subproductname>
            <c2pl:platformname>' . $Args{'platformname'} . '</c2pl:platformname>
            <c2pl:languagecode>' . $Args{'languagecode'} . '</c2pl:languagecode>
        </c2pl:GetBuildFromProfileRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetBuildFromProfileResponse", "build");
    return Adobe::Codex2::Types::build::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 GetBuildsFromProfile

    Usage:
    my @buildlist = $Codex->GetBuildsFromProfile(productname    => $ProductName,
                                                 versionname    => $VersionName,
                                                 profilename    => $ProfileName,
                                                 component      => $Component,
                                                 subproductname => $SubProjectName,
                                                 platformname   => $PlatformName,
                                                 languagecode   => $LanguageCode);

    Input:
    ...

    Output:
    ...

  <xs:element name="GetBuildsFromProfileRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:productname"/>
        <xs:element ref="schemas:versionname"/>
        <xs:element ref="schemas:profilename"/>
        <xs:element ref="schemas:component"/>
        <xs:element ref="schemas:subproductname"/>
        <xs:element ref="schemas:platformname"/>
        <xs:element ref="schemas:languagecode"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="GetBuildsFromProfileResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:buildlist"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  
=cut
# -------------------------------------------------------------------------
sub GetBuildsFromProfile
{
    # Get arguments.
    my $Self = shift;
    my %Args = (productname    => undef,
                versionname    => undef,
                profilename    => undef,
                component      => undef,
                subproductname => undef,
                platformname   => undef,
                languagecode   => undef,
                @_);

    # Check arguments.
    die("Adobe::Codex2::GetBuildsFromProfile - Error: 'productname' argument is not defined.\n")    unless (defined $Args{'productname'});
    die("Adobe::Codex2::GetBuildsFromProfile - Error: 'versionname' argument is not defined.\n")    unless (defined $Args{'versionname'});
    die("Adobe::Codex2::GetBuildsFromProfile - Error: 'profilename' argument is not defined.\n")    unless (defined $Args{'profilename'});
    die("Adobe::Codex2::GetBuildsFromProfile - Error: 'component' argument is not defined.\n")      unless (defined $Args{'component'});
    die("Adobe::Codex2::GetBuildsFromProfile - Error: 'subproductname' argument is not defined.\n") unless (defined $Args{'subproductname'});
    die("Adobe::Codex2::GetBuildsFromProfile - Error: 'platformname' argument is not defined.\n")   unless (defined $Args{'platformname'});
    die("Adobe::Codex2::GetBuildsFromProfile - Error: 'languagecode' argument is not defined.\n")   unless (defined $Args{'languagecode'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetBuildsFromProfileRequest>
            <c2pl:productname>' . $Args{'productname'} . '</c2pl:productname>
            <c2pl:versionname>' . $Args{'versionname'} . '</c2pl:versionname>
            <c2pl:profilename>' . $Args{'profilename'} . '</c2pl:profilename>
            <c2pl:component>' . $Args{'component'} . '</c2pl:component>
            <c2pl:subproductname>' . $Args{'subproductname'} . '</c2pl:subproductname>
            <c2pl:platformname>' . $Args{'platformname'} . '</c2pl:platformname>
            <c2pl:languagecode>' . $Args{'languagecode'} . '</c2pl:languagecode>
        </c2pl:GetBuildsFromProfileRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetBuildsFromProfileResponse", "buildlist");
    return Adobe::Codex2::Types::buildlist::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 GetBuildsFromProfileWithComponent

    Usage:
    my @buildlist = $Codex->GetBuildsFromProfileWithComponent(productname     => $ProductName,
                                                              versionname     => $VersionName,
                                                              profilename     => $ProfileName,
                                                              component       => $Component,
                                                              subproductname  => $SubProductName,
                                                              platformname    => $PlatformName,
                                                              languagecode    => $LanguageCode,
                                                              filtercomponent => $FilterComponent,
                                                              buildname       => $BuildName);
    
    Input:
    ...

    Output:
    ...

  <xs:element name="GetBuildsFromProfileWithComponentRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:productname"/>
        <xs:element ref="schemas:versionname"/>
        <xs:element ref="schemas:profilename"/>
        <xs:element ref="schemas:component"/>
        <xs:element ref="schemas:subproductname"/>
        <xs:element ref="schemas:platformname"/>
        <xs:element ref="schemas:languagecode"/>
        <xs:element ref="schemas:filtercomponent"/>
        <xs:element ref="schemas:buildname"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  
  <xs:element name="GetBuildsFromProfileWithComponentResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:buildlist"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>  
  
=cut
# -------------------------------------------------------------------------
sub GetBuildsFromProfileWithComponent
{
    # Get arguments.
    my $Self = shift;
    my %Args = (productname     => undef,
                versionname     => undef,
                profilename     => undef,
                component       => undef,
                subproductname  => undef,
                platformname    => undef,
                languagecode    => undef,
                filtercomponent => undef,
                buildname       => undef,
                @_);

    # Check arguments.
    die("Adobe::Codex2::GetBuildsFromProfileWithComponent - Error: 'productname' argument is not defined.\n")     unless (defined $Args{'productname'});
    die("Adobe::Codex2::GetBuildsFromProfileWithComponent - Error: 'versionname' argument is not defined.\n")     unless (defined $Args{'versionname'});
    die("Adobe::Codex2::GetBuildsFromProfileWithComponent - Error: 'profilename' argument is not defined.\n")     unless (defined $Args{'profilename'});
    die("Adobe::Codex2::GetBuildsFromProfileWithComponent - Error: 'component' argument is not defined.\n")       unless (defined $Args{'component'});
    die("Adobe::Codex2::GetBuildsFromProfileWithComponent - Error: 'subproductname' argument is not defined.\n")  unless (defined $Args{'subproductname'});
    die("Adobe::Codex2::GetBuildsFromProfileWithComponent - Error: 'platformname' argument is not defined.\n")    unless (defined $Args{'platformname'});
    die("Adobe::Codex2::GetBuildsFromProfileWithComponent - Error: 'languagecode' argument is not defined.\n")    unless (defined $Args{'languagecode'});
    die("Adobe::Codex2::GetBuildsFromProfileWithComponent - Error: 'filtercomponent' argument is not defined.\n") unless (defined $Args{'filtercomponent'});
    die("Adobe::Codex2::GetBuildsFromProfileWithComponent - Error: 'buildname' argument is not defined.\n")       unless (defined $Args{'buildname'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetBuildsFromProfileWithComponentRequest>
            <c2pl:productname>' . $Args{'productname'} . '</c2pl:productname>
            <c2pl:versionname>' . $Args{'versionname'} . '</c2pl:versionname>
            <c2pl:profilename>' . $Args{'profilename'} . '</c2pl:profilename>
            <c2pl:component>' . $Args{'component'} . '</c2pl:component>
            <c2pl:subproductname>' . $Args{'subproductname'} . '</c2pl:subproductname>
            <c2pl:platformname>' . $Args{'platformname'} . '</c2pl:platformname>
            <c2pl:languagecode>' . $Args{'languagecode'} . '</c2pl:languagecode>
            <c2pl:filtercomponent>' . $Args{'filtercomponent'} . '</c2pl:filtercomponent>
            <c2pl:buildname>' . $Args{'buildname'} . '</c2pl:buildname>
        </c2pl:GetBuildsFromProfileWithComponentRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetBuildsFromProfileWithComponentResponse", "buildlist");
    return Adobe::Codex2::Types::buildlist::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 GetBuildsUsing

    Usage:
    my @buildlist = $Codex->GetBuildsUsing(id => $ID);

    Input:
    ...

    Output:
    ...

  <xs:element name="GetBuildsUsingRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="GetBuildsUsingResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:buildlist"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element> 

=cut
# -------------------------------------------------------------------------
sub GetBuildsUsing
{
    # Get arguments.
    my $Self = shift;
    my %Args = (id => undef,
                @_);
    
    # Check arguments.
    die("Adobe::Codex2::GetBuildsUsing - Error: 'id' argument is not defined.\n") unless (defined $Args{'id'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetBuildsUsingRequest>
            <c2pl:id>' . $Args{'id'} . '</c2pl:id>
        </c2pl:GetBuildsUsingRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetBuildsUsingResponse", "buildlist");
    return Adobe::Codex2::Types::buildlist::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 SetCertLevel
    
    Usage:
    my %build = $Codex->SetCertLevel(id              => $ID,
                                     certlevel       => $CertLevel,
                                     ldapcredentials => {userid => $UserID, password => $Password});
    
    Input:
    ...

    Output:
    ...

  <xs:element name="SetCertLevelRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
        <xs:element ref="schemas:certlevel"/>
        <xs:element ref="schemas:ldapcredentials"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="SetCertLevelResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:build"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub SetCertLevel
{
    # Get arguments.
    my $Self = shift;
    my %Args = (id              => undef,
                certlevel       => undef,
                ldapcredentials => {userid   => undef,
                                    password => undef},
                @_);

    # Check arguments.
    die("Adobe::Codex2::SetCertLevel - Error: 'id' argument is not defined.\n")            unless (defined $Args{'id'});
    die("Adobe::Codex2::SetCertLevel - Error: 'certlevel' argument is not defined.\n")     unless (defined $Args{'certlevel'});
    die("Adobe::Codex2::SetCertLevel - Error: LDAP 'userid' argument is not defined.\n")   unless (defined $Args{'ldapcredentials'}->{'userid'});
    die("Adobe::Codex2::SetCertLevel - Error: LDAP 'password' argument is not defined.\n") unless (defined $Args{'ldapcredentials'}->{'password'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:SetCertLevelRequest>
            <c2pl:id>' . $Args{'id'} . '</c2pl:id>
            <c2pl:certlevel>' . $Args{'certlevel'} . '</c2pl:certlevel>
            <c2pl:ldapcredentials>
                <c2pl:userid>' . $Args{'ldapcredentials'}->{'userid'} . '</c2pl:userid>
                <c2pl:password>' . $Args{'ldapcredentials'}->{'password'} . '</c2pl:password>
            </c2pl:ldapcredentials>
        </c2pl:SetCertLevelRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "SetCertLevelResponse", "build");
    return Adobe::Codex2::Types::build::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 SetLocation

    Usage:
    my %build = $Codex->SetLocation(id              => $ID,
                                    uri             => $URI,
                                    ldapcredentials => {userid => $UserID, password => $Password});
    
    Input:
    ...

    Output:
    ...

  <xs:element name="SetLocationRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
        <xs:element ref="schemas:uri"/>
        <xs:element ref="schemas:ldapcredentials"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="SetLocationResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:build"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub SetLocation
{
    # Get arguments.
    my $Self = shift;
    my %Args = (id              => undef,
                uri             => undef,
                ldapcredentials => {userid   => undef,
                                    password => undef},
                @_);

    # Check arguments.
    die("Adobe::Codex2::SetLocation - Error: 'id' argument is not defined.\n")            unless (defined $Args{'id'});
    die("Adobe::Codex2::SetLocation - Error: 'uri' argument is not defined.\n")           unless (defined $Args{'uri'});
    die("Adobe::Codex2::SetLocation - Error: LDAP 'userid' argument is not defined.\n")   unless (defined $Args{'ldapcredentials'}->{'userid'});
    die("Adobe::Codex2::SetLocation - Error: LDAP 'password' argument is not defined.\n") unless (defined $Args{'ldapcredentials'}->{'password'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:SetLocationRequest>
            <c2pl:id>' . $Args{'id'} . '</c2pl:id>
            <c2pl:uri>' . $Args{'uri'} . '</c2pl:uri>
            <c2pl:ldapcredentials>
                <c2pl:userid>' . $Args{'ldapcredentials'}->{'userid'} . '</c2pl:userid>
                <c2pl:password>' . $Args{'ldapcredentials'}->{'password'} . '</c2pl:password>
            </c2pl:ldapcredentials>
        </c2pl:SetLocationRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "SetLocationResponse", "build");
    return Adobe::Codex2::Types::build::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 SetMetadata

    Usage:
    my %build = $Codex->SetMetadata(id              => $ID,
                                    key             => $Key,
                                    value           => $Value,
                                    ldapcredentials => {userid => $UserID, password => $Password});
    
    Input:
    ...

    Output:
    ...

  <xs:element name="SetMetadataRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
        <xs:element ref="schemas:key"/>
        <xs:element ref="schemas:value"/>
        <xs:element ref="schemas:ldapcredentials"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="SetMetadataResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:build"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub SetMetadata
{
    # Get arguments.
    my $Self = shift;
    my %Args = (id              => undef,
                key             => undef,
                value           => undef,
                ldapcredentials => {userid   => undef,
                                    password => undef},
                @_);

    # Check arguments.
    die("Adobe::Codex2::SetMetadata - Error: 'id' argument is not defined.\n")            unless (defined $Args{'id'});
    die("Adobe::Codex2::SetMetadata - Error: 'key' argument is not defined.\n")           unless (defined $Args{'key'});
    die("Adobe::Codex2::SetMetadata - Error: 'value' argument is not defined.\n")         unless (defined $Args{'value'});
    die("Adobe::Codex2::SetMetadata - Error: LDAP 'userid' argument is not defined.\n")   unless (defined $Args{'ldapcredentials'}->{'userid'});
    die("Adobe::Codex2::SetMetadata - Error: LDAP 'password' argument is not defined.\n") unless (defined $Args{'ldapcredentials'}->{'password'});

    # Escape special characters.
    $Args{'key'} =~ s/&/&amp;/g;
    $Args{'key'} =~ s/</&lt;/g;
    $Args{'key'} =~ s/>/&gt;/g;

    $Args{'value'} =~ s/&/&amp;/g;
    $Args{'value'} =~ s/</&lt;/g;
    $Args{'value'} =~ s/>/&gt;/g;

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:SetMetadataRequest>
            <c2pl:id>' . $Args{'id'} . '</c2pl:id>
            <c2pl:key>' . $Args{'key'} . '</c2pl:key>
            <c2pl:value>' . $Args{'value'} . '</c2pl:value>
            <c2pl:ldapcredentials>
                <c2pl:userid>' . $Args{'ldapcredentials'}->{'userid'} . '</c2pl:userid>
                <c2pl:password>' . $Args{'ldapcredentials'}->{'password'} . '</c2pl:password>
            </c2pl:ldapcredentials>
        </c2pl:SetCertLevelRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "SetMetadataResponse", "build");
    return Adobe::Codex2::Types::build::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 SetStatus
    
    Usage:
    my %build = $Codex->SetStatus(id              => $ID,
                                  statusname      => $StatusName,
                                  ldapcredentials => {userid => $UserID, password => $Password});
    
    Input:
    ...

    Output:
    ...

  <xs:element name="SetStatusRequest">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
        <xs:element ref="schemas:statusname"/>
        <xs:element ref="schemas:ldapcredentials"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="SetStatusResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:build"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub SetStatus
{
    # Get arguments.
    my $Self = shift;
    my %Args = (id              => undef,
                statusname      => undef,
                ldapcredentials => {userid   => undef,
                                    password => undef},
                @_);

    # Check arguments.
    die("Adobe::Codex2::SetStatus - Error: 'id' argument is not defined.\n")            unless (defined $Args{'id'});
    die("Adobe::Codex2::SetStatus - Error: 'statusname' argument is not defined.\n")    unless (defined $Args{'statusname'});
    die("Adobe::Codex2::SetStatus - Error: LDAP 'userid' argument is not defined.\n")   unless (defined $Args{'ldapcredentials'}->{'userid'});
    die("Adobe::Codex2::SetStatus - Error: LDAP 'password' argument is not defined.\n") unless (defined $Args{'ldapcredentials'}->{'password'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:SetStatusRequest>
            <c2pl:id>' . $Args{'id'} . '</c2pl:id>
            <c2pl:statusname>' . $Args{'statusname'} . '</c2pl:statusname>
            <c2pl:ldapcredentials>
                <c2pl:userid>' . $Args{'ldapcredentials'}->{'userid'} . '</c2pl:userid>
                <c2pl:password>' . $Args{'ldapcredentials'}->{'password'} . '</c2pl:password>
            </c2pl:ldapcredentials>
        </c2pl:SetStatusRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "SetStatusResponse", "build");
    return Adobe::Codex2::Types::build::Data($ResponseData);
}

1;
__END__

=pod

=head1 AUTHOR

    Dave Foglesong (fogleson@adobe.com)

=head1 COPYRIGHT

    Copyright 2008 Adobe Systems Incorporated. All rights reserved.

=cut


