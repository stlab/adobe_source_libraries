# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2::Metadata - Perl interface to Codex 2.0 system.

=head1 DESCRIPTION

See POD from Adobe::Codex2 for details on using this module.

=head1 SUBROUTINES/METHODS
    
    AddGlobalMetadataKey
    AddMetadataKey
    DeleteGlobalMetadataKey
    DeleteMetadataKey
    GetMetadataKeyByID
    GetMetadataKeys

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2::Metadata;

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
use Adobe::Codex2::Types::id;
use Adobe::Codex2::Types::metadatakey;
use Adobe::Codex2::Types::metadatakeylist;

# -------------------------------------------------------------------------
=pod

=head2 AddGlobalMetadataKey

    Usage:
    my $id = $Codex->AddGlobalMetadataKey(key             => $Key,
                                          ldapcredentials => {userid => $UserID, password => $Password});
    
    Input:
    ...

    Output:
    ...

  <xs:element name="AddGlobalMetadataKeyRequest">
    <xs:complexType>
      <xs:sequence>
        <xs:element ref="schemas:key"/>
        <xs:element ref="schemas:ldapcredentials"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  
  <xs:element name="AddGlobalMetadataKeyResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>  

=cut
# -------------------------------------------------------------------------
sub AddGlobalMetadataKey
{
    # Get arguments.
    my $Self = shift;
    my %Args = (key             => undef,
                ldapcredentials => {userid   => undef,
                                    password => undef},
                @_);
    
    # Check arguments.
    die("Adobe::Codex2::AddGlobalMetadataKey - Error: 'key' argument is not defined.\n")           unless (defined $Args{'key'});
    die("Adobe::Codex2::AddGlobalMetadataKey - Error: LDAP 'userid' argument is not defined.\n")   unless (defined $Args{'ldapcredentials'}->{'userid'});
    die("Adobe::Codex2::AddGlobalMetadataKey - Error: LDAP 'password' argument is not defined.\n") unless (defined $Args{'ldapcredentials'}->{'password'});
    
    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:AddGlobalMetadataKeyRequest>
            <c2pl:key>' . $Args{'key'} . '</c2pl:key>
            <c2pl:ldapcredentials>
                <c2pl:userid>' . $Args{'ldapcredentials'}->{'userid'} . '</c2pl:userid>
                <c2pl:password>' . $Args{'ldapcredentials'}->{'password'} . '</c2pl:password>
            </c2pl:ldapcredentials>
        </c2pl:AddGlobalMetadataKeyRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "AddGlobalMetadataKeyResponse", "id");
    return Adobe::Codex2::Types::id::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod
    
=head2 AddMetadataKey
    
    Usage:
    my $id = $Codex->AddMetadataKey(key             => $Key,
                                    productname     => $ProductName,
                                    versionname     => $VersionName,
                                    ldapcredentials => {userid => $UserID, password => $Password});
    
    Input:
    ...

    Output:
    ...

  <xs:element name="AddMetadataKeyRequest">
    <xs:complexType>
      <xs:sequence>
        <xs:element ref="schemas:key"/>
        <xs:element ref="schemas:productname"/>
        <xs:element ref="schemas:versionname"/>
        <xs:element ref="schemas:ldapcredentials"/>        
      </xs:sequence>
    </xs:complexType>
  </xs:element>

  <xs:element name="AddMetadataKeyResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=cut
# -------------------------------------------------------------------------
sub AddMetadataKey
{
    # Get arguments.
    my $Self = shift;
    my %Args = (key             => undef,
                productname     => "",
                versionname     => "",
                ldapcredentials => {userid   => undef,
                                    password => undef},
                @_);

    # Check arguments.
    die("Adobe::Codex2::AddMetadataKey - Error: 'key' argument is not defined.\n")           unless (defined $Args{'key'});
    die("Adobe::Codex2::AddMetadataKey - Error: 'productname' argument is not defined.\n")   unless (defined $Args{'productname'});
    die("Adobe::Codex2::AddMetadataKey - Error: 'versionname' argument is not defined.\n")   unless (defined $Args{'versionname'});
    die("Adobe::Codex2::AddMetadataKey - Error: LDAP 'userid' argument is not defined.\n")   unless (defined $Args{'ldapcredentials'}->{'userid'});
    die("Adobe::Codex2::AddMetadataKey - Error: LDAP 'password' argument is not defined.\n") unless (defined $Args{'ldapcredentials'}->{'password'});
    
    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:AddMetadataKeyRequest>
            <c2pl:key>' . $Args{'key'} . '</c2pl:key>
            <c2pl:productname>' . $Args{'productname'} . '</c2pl:productname>
            <c2pl:versionname>' . $Args{'versionname'} . '</c2pl:versionname>
            <c2pl:ldapcredentials>
                <c2pl:userid>' . $Args{'ldapcredentials'}->{'userid'} . '</c2pl:userid>
                <c2pl:password>' . $Args{'ldapcredentials'}->{'password'} . '</c2pl:password>
            </c2pl:ldapcredentials>
        </c2pl:AddMetadataKeyRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "AddMetadataKeyResponse", "id");
    return Adobe::Codex2::Types::id::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 DeleteGlobalMetadataKey
    
    Usage:
    my $result = $Codex->DeleteGlobalMetadataKey(id              => $ID,
                                                 ldapcredentials => {userid => $UserID, password => $Password});
    
    Input:
    ...

    Output:
    ...

  <xs:element name="DeleteGlobalMetadataKeyRequest">
    <xs:complexType>
      <xs:sequence>
        <xs:element ref="schemas:id"/>
        <xs:element ref="schemas:ldapcredentials"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  
  <xs:element name="DeleteGlobalMetadataKeyResponse">
    <xs:complexType>
      <xs:sequence>
        <xs:element ref="schemas:operationresult"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>    

=cut
# -------------------------------------------------------------------------
sub DeleteGlobalMetadataKey
{
    # Get arguments.
    my $Self = shift;
    my %Args = (id              => undef,
                ldapcredentials => {userid   => undef,
                                    password => undef},
                @_);

    # Check arguments.
    die("Adobe::Codex2::DeleteGlobalMetadataKey - Error: 'id' argument is not defined.\n")            unless (defined $Args{'id'});
    die("Adobe::Codex2::DeleteGlobalMetadataKey - Error: LDAP 'userid' argument is not defined.\n")   unless (defined $Args{'ldapcredentials'}->{'userid'});
    die("Adobe::Codex2::DeleteGlobalMetadataKey - Error: LDAP 'password' argument is not defined.\n") unless (defined $Args{'ldapcredentials'}->{'password'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:DeleteGlobalMetadataKeyRequest>
            <c2pl:id>' . $Args{'id'} . '</c2pl:id>
            <c2pl:ldapcredentials>
                <c2pl:userid>' . $Args{'ldapcredentials'}->{'userid'} . '</c2pl:userid>
                <c2pl:password>' . $Args{'ldapcredentials'}->{'password'} . '</c2pl:password>
            </c2pl:ldapcredentials>
        </c2pl:DeleteGlobalMetadataKeyRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "DeleteGlobalMetadataKeyResponse", "operationresult");
    return Adobe::Codex2::Types::operationresult::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 DeleteMetadataKey
    
    Usage:
    my $result = $Codex->DeleteMetadataKey(id              => $ID,
                                           ldapcredentials => {userid => $UserID, password => $Password});
    
    Input:
    ...

    Output:
    ...

  <xs:element name="DeleteMetadataKeyRequest">
    <xs:complexType>
      <xs:sequence>
        <xs:element ref="schemas:id"/>
        <xs:element ref="schemas:ldapcredentials"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  
  <xs:element name="DeleteMetadataKeyResponse">
    <xs:complexType>
      <xs:sequence>
        <xs:element ref="schemas:operationresult"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>  
  
=cut
# -------------------------------------------------------------------------
sub DeleteMetadataKey
{
    # Get arguments.
    my $Self = shift;
    my %Args = (id              => undef,
                ldapcredentials => {userid   => undef,
                                    password => undef},
                @_);

    # Check arguments.
    die("Adobe::Codex2::DeleteMetadataKey - Error: 'id' argument is not defined.\n")            unless (defined $Args{'id'});
    die("Adobe::Codex2::DeleteMetadataKey - Error: LDAP 'userid' argument is not defined.\n")   unless (defined $Args{'ldapcredentials'}->{'userid'});
    die("Adobe::Codex2::DeleteMetadataKey - Error: LDAP 'password' argument is not defined.\n") unless (defined $Args{'ldapcredentials'}->{'password'});

    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:DeleteMetadataKeyRequest>
            <c2pl:id>' . $Args{'id'} . '</c2pl:id>
            <c2pl:ldapcredentials>
                <c2pl:userid>' . $Args{'ldapcredentials'}->{'userid'} . '</c2pl:userid>
                <c2pl:password>' . $Args{'ldapcredentials'}->{'password'} . '</c2pl:password>
            </c2pl:ldapcredentials>
        </c2pl:DeleteMetadataKeyRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "DeleteMetadataKeyResponse", "operationresult");
    return Adobe::Codex2::Types::operationresult::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 GetMetadataKeyById
    
    Usage:
    my $metadatakey = $Codex->GetMetadataKeyById(id => $ID);

    Input:
    ...

    Output:
    ...

  <xs:element name="GetMetadataKeyByIdRequest">
    <xs:complexType>
      <xs:sequence>
        <xs:element ref="schemas:id"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  
  <xs:element name="GetMetadataKeyByIdResponse">
    <xs:complexType>
      <xs:sequence>
        <xs:element ref="schemas:metadatakey"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>    

=cut
# -------------------------------------------------------------------------
sub GetMetadataKeyById
{
    # Get arguments.
    my $Self = shift;
    my %Args = (id => undef,
                @_);

    # Check arguments.
    die("Adobe::Codex2::GetMetadataKeyById - Error: 'id' argument is not defined.\n") unless (defined $Args{'id'});
    
    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetMetadataKeyByIdRequest >
            <c2pl:id>' . $Args{'id'} . '</c2pl:id>
        </c2pl:GetMetadataKeyByIdRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetMetadataKeyByIdResponse", "metadatakey");
    return Adobe::Codex2::Types::metadatakey::Data($ResponseData);
}

# -------------------------------------------------------------------------
=pod

=head2 GetMetadataKeys
    
    Usage:
    my @metadatakeylist = $Codex->GetMetadataKeys(productname => $Product,
                                                  versionname => $Version);

    Input:
    ...

    Output:
    ...

  <xs:element name="GetMetadataKeysRequest">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" ref="schemas:productname"/>
        <xs:element minOccurs="0" ref="schemas:versionname"/>
      </xs:sequence>      
    </xs:complexType>
  </xs:element>

  <xs:element name="GetMetadataKeysResponse">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:metadatakeylist"/>
        <xs:element minOccurs="0" ref="schemas:performancedata"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  
=cut
# -------------------------------------------------------------------------
sub GetMetadataKeys
{
    # Get arguments.
    my $Self = shift;
    my %Args = (productname => "",
                versionname => "",
                @_);

    # Check arguments.
    die("Adobe::Codex2::GetMetadataKeys - Error: 'productname' argument is not defined.\n") unless (defined $Args{'productname'});
    die("Adobe::Codex2::GetMetadataKeys - Error: 'versionname' argument is not defined.\n") unless (defined $Args{'versionname'});
    
    # Make SOAP call.
    my $ResponseXML = Adobe::Codex2::SOAP::Call(
        '<c2pl:GetMetadataKeysRequest>
            <c2pl:productname>' . $Args{'productname'} . '</c2pl:productname>
            <c2pl:versionname>' . $Args{'versionname'} . '</c2pl:versionname>
        </c2pl:GetMetadataKeysRequest>');
    
    # Process response XML.
    my $ResponseData = Adobe::Codex2::SOAP::XML2Data($ResponseXML, "GetMetadataKeysResponse", "metadatakeylist");
    return Adobe::Codex2::Types::metadatakeylist::Data($ResponseData);
}

1;
__END__

=pod

=head1 AUTHOR

    Dave Foglesong (fogleson@adobe.com)

=head1 COPYRIGHT

    Copyright 2008 Adobe Systems Incorporated. All rights reserved.

=cut


