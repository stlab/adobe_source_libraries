# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2 - Perl interface to Codex 2.0 system.

=head1 SYNOPSIS

    use Adobe::Codex2;
    my $Codex = Adobe::Codex2->new();

    my @productlist = $Codex->GetProducts();
    ...

=head1 DESCRIPTION

Adobe::Codex2 is the main module to use for working with the Codex 2.0 system.
This is the only Adobe::Codex2 module you'll need to include in your scripts;
the rest of the files in the Adobe/Codex/... tree are used indirectly by the
main module.

However, you will want to look at the POD for the other modules in the tree,
since all the methods are documented in those modules.

=head2 CALLING CONVENTIONS

There is a 1:1 mapping between the methods in the Codex WSDL and the methods
in the Perl module. This includes the arguments; the names of the attributes
and elements in the WSDL are used as the argument names for the Perl methods.

For example, here is the XML for the GetContacts request and response:

    <xs:element name="GetContactsRequest">
      <xs:complexType>
        <xs:sequence maxOccurs="1" minOccurs="1">
          <xs:element ref="schemas:versionname"/>
          <xs:element ref="schemas:productname"/>
        </xs:sequence>
      </xs:complexType>
    </xs:element>

    <xs:element name="GetContactsResponse">
      <xs:complexType>
        <xs:sequence maxOccurs="1" minOccurs="1">
          <xs:element ref="schemas:contactlist"/>
          <xs:element minOccurs="0" ref="schemas:performancedata"/>
        </xs:sequence>
      </xs:complexType>
    </xs:element>

And here is the Perl method:

    my @contactlist = $Codex->GetContacts(productname => $ProductName,
                                          versionname => $VersionName);

Remember that named arguments can be put in the call in any order.

In general, all arguments are required for all methods. If you want to
use an empty value for an argument, pass in an empty string ("").

=head2 RETURN DATA

In general, return data will be one of the following:

    "operationresult" = boolean value, expressed as the string "true"
    or "false". This is most often returned from "delete" methods that
    remove an item from the Codex DB.

    ID value = integer. This is most often returned from "add" methods
    that insert a new item in the Codex DB.

    Data = hash. This is what you'll mostly get from Codex methods.
    The hash keys will map to the return data names in the WSDL.

    List = array of data hash references. This is what you'll get when
    you call most of the "Get" methods that work on an entire set of
    items (e.g., "GetProducts").

All the Codex calls also return "performancedata" with data about how long
the call took to run. This data is currently discarded by the Perl module.

=head1 VARIABLES/CONFIGURATION

    $DEBUG = 1/0 (on/off), default is 0 (off).
    
    Print debugging information. Currently this just prints the SOAP
    request and response XML. This needs to be set before you create
    the Codex2 object.

    $TIMEOUT = 180

    Sets timeout for LWP::UserAgent calls to Codex. Default is 180
    seconds, the same as the internal default in LWP::UserAgent.

=head1 DEPENDENCIES

In an attempt to placate those who don't want to install any extra modules in
their Perl distribution, this module depends on very few other modules. With
the exception of XML::Parser, all of these should be included in a base Perl
distribution:

    HTTP::Headers;
    HTTP::Request;
    LWP::UserAgent;
    XML::Parser

And of course, it also depends on the entire Adobe::Codex2::* module tree.

=head1 SUBROUTINES/METHODS

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2;

require 5.8.0;
use strict;
use warnings;

use base qw(Exporter);
our $VERSION     = "0.2.0";
our @ISA         = qw(Adobe::Codex2::Build
                      Adobe::Codex2::Metadata);
our @EXPORT      = qw();
our @EXPORT_OK   = qw($DEBUG
                      $TIMEOUT);
our %EXPORT_TAGS = ();

# Get Perl modules.
# None.

# Get Codex modules.
use Adobe::Codex2::Build;
use Adobe::Codex2::Metadata;
use Adobe::Codex2::SOAP;

# -------------------------------------------------------------------------
# Module variables.
# -------------------------------------------------------------------------
our $DEBUG = 0;
our $TIMEOUT = 180;

# -------------------------------------------------------------------------
=pod

=head2 new

    Create new Codex object.

    Usage:
    my $Codex = Adobe::Codex2->new($Server);

    Input:
    $Server = FQDN for Codex server (optional).

    Output:
    success = Codex object.
    failure = undef or die() with error text.
    
    Notes:
    * Default $Server value is "codex.corp.adobe.com:8080".

=cut
# -------------------------------------------------------------------------
sub new
{
    # Get arguments.
    my $Class  = shift;
    my $Server = shift;
    
    # Check arguments.
    $Server = "codex.corp.adobe.com:8080" unless (defined $Server);
    
    # Service endpoint.
    my $Service = "http://$Server/codex/services";

    # Set up object.
    my $Self = {};
    bless $Self, ref($Class) || $Class;
    
    # Set values in SOAP module.
    $Adobe::Codex2::SOAP::DEBUG   = $DEBUG;
    $Adobe::Codex2::SOAP::SERVICE = $Service;
    $Adobe::Codex2::SOAP::TIMEOUT = $TIMEOUT;

    # Return object.
    return $Self;
}

# -------------------------------------------------------------------------
=pod
    
=head2 DESTROY
    
    Object destructor.
    Called automatically when object goes out of scope; should not be
    called directly by user.

=cut
# -------------------------------------------------------------------------
sub DESTROY
{
    # Get arguments.
    my $Self = shift;

    # Check arguments.
    # TODO

    # Return something.
    return 1;
}

1;
__END__

=pod

=head1 AUTHOR

    Dave Foglesong (fogleson@adobe.com)

=head1 COPYRIGHT

    Copyright 2008 Adobe Systems Incorporated. All rights reserved.

=cut

