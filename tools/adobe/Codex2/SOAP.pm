# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2::SOAP - Perl interface to Codex 2.0 system.

=head1 DESCRIPTION

See POD from Adobe::Codex2 for details on using this module.

This module contains all the routines that interact with the Codex SOAP
endpoint. This module makes the SOAP call, parses the returned XML for
SOAP faults, and extracts results data from the returned XML.

=head1 VARIABLES/CONFIGURATION

    $DEBUG = Print request/response XML.
    $TIMEOUT = Timeout for LWP::UserAgent.

=head1 SUBROUTINES/METHODS

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2::SOAP;

require 5.8.0;
use strict;
use warnings;

use base qw(Exporter);
our $VERSION     = "0.2.0";
our @ISA         = qw();
our @EXPORT      = qw();
our @EXPORT_OK   = qw(&Call
                      &XML2Data
                      $DEBUG
                      $SERVICE
                      $TIMEOUT);
our %EXPORT_TAGS = ();

# Get Perl modules.
use HTTP::Headers;
use HTTP::Request;
use LWP::UserAgent;
use XML::Parser;

use Data::Dumper;
$Data::Dumper::Indent = 1;

# Get Codex modules.
# None.

# -------------------------------------------------------------------------
# Module variables.
# -------------------------------------------------------------------------
my $CurrentElement;
my $Fault;

our $DEBUG   = 0;
our $SERVICE = "__UNSET__";
our $TIMEOUT = 180;

# -------------------------------------------------------------------------
=pod

=head2 Call
    
    Make SOAP call.

    Usage:
    my $ResponseXML = Call($BodyXML, $HeaderXML);

    Input:
    $BodyXML   = XML fragment for web service <Body> element.
    $HeaderXML = XML fragment for web service <Header> element (optional).

    Output:
    success = XML returned from web service method.
    failure = die() with error text.

    Notes:
    none

=cut
# -------------------------------------------------------------------------
sub Call
{
    # Get arguments.
    my $BodyXML   = shift;
    my $HeaderXML = shift;

    # Check arguments.
    die("Adobe::Codex2::SOAP::Call - Error: 'BodyXML' is not defined.\n") unless (defined $BodyXML);

    # $HeaderXML is optional, but make sure it's defined.
    $HeaderXML = "" unless (defined $HeaderXML);
    
    # Check that $SERVICE is set.
    die("Adobe::Codex2::SOAP::Call - Error: 'SERVICE' is not set.\n") if ($SERVICE eq "__UNSET__");

    # Create SOAP XML.
    my $RequestXML = <<END_OF_XML;
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
                   xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
                   xmlns:xsd="http://www.w3.org/2001/XMLSchema"
                   xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
    <SOAP-ENV:Header>$HeaderXML</SOAP-ENV:Header>
    <SOAP-ENV:Body xmlns:c2pl="http://codex.corp.adobe.com/codex/schemas">
        $BodyXML
    </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
END_OF_XML
    
    # DEBUG: Print request XML.
    if ($DEBUG)
    {
        print "*** RequestXML ***\n",
              "$SERVICE\n",
              "$RequestXML\n";
    }

    # Create UserAgent object.
    my $UserAgent = LWP::UserAgent->new();
    
    # Set timeout value.
    $UserAgent->timeout($TIMEOUT);

    # Create HTTP request.
    my $HTTPHeaders = HTTP::Headers->new('content-type' => 'text/xml');
    my $HTTPRequest = HTTP::Request->new('POST', $SERVICE, $HTTPHeaders, $RequestXML);

    # Send it to the server.
    my $Start = time();
    my $HTTPResponse = $UserAgent->request($HTTPRequest);
    my $End = time();
    my $Total = $End - $Start;

    print "*** TIMEOUT = $TIMEOUT ***\n",
          "START = $Start\n",
          "END   = $End\n",
          "TOTAL = $Total\n\n" if ($DEBUG);

    # Get response content.
    my $Content = $HTTPResponse->content();
    
    # DEBUG: Print response XML.
    if ($DEBUG)
    {
        # Add newlines to response XML to make it readable.
        my $DebugContent = $Content;
        $DebugContent =~ s/>/>\n/g;

        print "*** ResponseXML ***\n",
              "$DebugContent\n";
    }

    # Check for errors.
    my ($Status, $Error) = Adobe::Codex2::SOAP::ParseFault($Content);
    
    # Get name of calling routine.
    my @CallData = caller(1);
    my $Routine  = (split(/::/, $CallData[3]))[-1];

    # Branch based on status.
    if ($Status eq "ERROR")
    {
        die("Adobe::Codex2::SOAP::$Routine - Error: " . $HTTPResponse->status_line() . "\n");
    }
    elsif ($Status eq "FAULT")
    {
        my $Fault = "SOAP fault:\n";
        $Fault .= "Fault actor  = " . $Error->{'actor'}  . "\n" if (exists $Error->{'actor'});
        $Fault .= "Fault code   = " . $Error->{'code'}   . "\n" if (exists $Error->{'code'});
        $Fault .= "Fault detail = " . $Error->{'detail'} . "\n" if (exists $Error->{'detail'});
        $Fault .= "Fault string = " . $Error->{'string'} . "\n" if (exists $Error->{'string'});

        die("Adobe::Codex2::SOAP::$Routine - $Fault\n");
    }
    else # Assume success, return content (XML).
    {
        return $Content;
    }
}

# -------------------------------------------------------------------------
=pod

=head2 ParseFault

    Parse XML for SOAP fault.

    TBA

=cut
# -------------------------------------------------------------------------
sub ParseFault
{
    # Get arguments.
    my $XML = shift;

    # Check arguments.
    die("Adobe::Codex2::SOAP::ParseFault - Error: 'XML' is not defined.\n") unless (defined $XML);

    # Reset module variables.
    undef $CurrentElement;
    undef $Fault;

    # Set up XML::Parser.
    my $XMLParser = XML::Parser->new(Handlers => {Start => \&FaultStartHandler,
                                                  End   => \&FaultEndHandler,
                                                  Char  => \&FaultCharHandler});
    
    # Try to parse the XML.
    eval { $XMLParser->parse($XML); };
    
    # Parsing error (e.g., web service returned HTML vs. XML).
    return("ERROR", $@) if ($@);
    
    # Found SOAP fault.
    return("FAULT", $Fault) if (keys %{$Fault});

    # Success.
    return("SUCCESS", $XML);
}

# -------------------------------------------------------------------------
# Helper routines for XML::Parser.
# -------------------------------------------------------------------------

sub FaultStartHandler
{
    # Get arguments.
    my $Expat   = shift;
    my $Element = shift;
    my %Attr    = @_;
    
    # Save current element value.
    $CurrentElement = $Element;

    # Return something.
    return 1;
}

sub FaultEndHandler
{
    # Get arguments.
    my $Expat   = shift;
    my $Element = shift;
    
    # Clear current element value.
    undef $CurrentElement;

    # Return something.
    return 1;
}

sub FaultCharHandler
{
    # Get arguments.
    my $Expat    = shift;
    my $Contents = shift;
    
    # Shouldn't ever hit this.
    return unless (defined $CurrentElement);

    # Process contents.
    chomp($Contents);
    if ($CurrentElement eq "faultactor")
    {
        $Fault->{'actor'} .= "\n" if ($Fault->{'actor'});
        $Fault->{'actor'} .= $Contents;
    }
    elsif ($CurrentElement eq "faultcode")
    {
        $Fault->{'code'} .= "\n" if ($Fault->{'code'});
        $Fault->{'code'} .= $Contents;
    }
    elsif ($CurrentElement eq "faultdetail")
    {
        $Fault->{'detail'} .= "\n" if ($Fault->{'detail'});
        $Fault->{'detail'} .= $Contents;
    }
    elsif ($CurrentElement eq "faultstring")
    {
        $Fault->{'string'} .= "\n" if ($Fault->{'string'});
        $Fault->{'string'} .= $Contents;
    }
    
    # Return something.
    return 1;
}

# -------------------------------------------------------------------------
# XML2Data
# Convert response XML to Perl data.
# -------------------------------------------------------------------------
sub XML2Data
{
    # Get arguments.
    my $XML      = shift;
    my $Response = shift;
    my $Type     = shift;
    
    # Check arguments.
    die("Adobe::Codex2::SOAP::XML2Data - Error: 'XML' is not defined.\n")      unless (defined $XML);
    die("Adobe::Codex2::SOAP::XML2Data - Error: 'Response' is not defined.\n") unless (defined $Response);
    die("Adobe::Codex2::SOAP::XML2Data - Error: 'Type' is not defined.\n")     unless (defined $Type);

    # Set up XML::Parser.
    my $XMLParser = XML::Parser->new(Style => 'Tree');
    my $Tree = eval { $XMLParser->parse($XML); };

    # Parsing error (shouldn't get this, since this XML has been parsed).
    die("Adobe::Codex2::SOAP::XML2Data - Error: Got parsing error: $@\n") if ($@);
    
    # Parse the tree.
    my $Data = ParseTree({}, @{$Tree});
    
    # DEBUG: Print tree data.
    if ($DEBUG)
    {
        print "*** Tree Data ***\n",
              Dumper $Data,
              "\n";
    }

    # Check for response data.
    die("Adobe::Codex2::SOAP::XML2Data - Error: '$Response' not found in XML data.\n")
        unless (exists $Data->{'soapenv:Envelope'}->{'soapenv:Body'}->{$Response});
    
    die("Adobe::Codex2::SOAP::XML2Data - Error: '$Type' not found in response data.\n")
        unless (exists $Data->{'soapenv:Envelope'}->{'soapenv:Body'}->{$Response}->{$Type});
    
    # Return results.
    return $Data->{'soapenv:Envelope'}->{'soapenv:Body'}->{$Response}->{$Type};
}

# -------------------------------------------------------------------------
# ParseTree
# Helper routine for ParseXML.
#
# Adapted from XML::Simple collapse() routine.
#
# Nested elements can appear as:
# $VAR = {
#   'empty' = {},
#   'single' = {
#     'animal' => 'dog'
#   },
#   'many' = [
#     { 'animal' => 'cat' },
#     { 'animal' => 'cow' }
#   ]
# }
# -------------------------------------------------------------------------
sub ParseTree
{
    # Get attributes.
    my $Attr = shift;
    
    # Process rest of args.
    while(@_)
    {
        my $Key = shift;
        my $Val = shift;

        if (ref($Val))
        {
            $Val = ParseTree(@{$Val});
        }
        elsif ($Key eq '0')
        {
            # Skip whitespace content.
            next if ($Val =~ m/^\s*$/s);

            # Collapse text content in element with no attributes.:w
            return $Val unless (%{$Attr} or (@_));
            
            # Fix '0' key.
            $Key = "__content__";
        }
        else
        {
            warn("Found odd key case: Key = $Key");
        }
        
        # Add/update data.
        if (exists $Attr->{$Key})
        {
            if (ref($Attr->{$Key}) eq "ARRAY")
            {
                push(@{$Attr->{$Key}}, $Val);
            }
            else
            {
                $Attr->{$Key} = [$Attr->{$Key}, $Val];
            }
        }
        else
        {
            $Attr->{$Key} = $Val;
        }
    }
    
    # Return results.
    return ($Attr);
}

1;
__END__

=pod

=head1 AUTHOR

    Dave Foglesong (fogleson@adobe.com)

=head1 COPYRIGHT

    Copyright 2008 Adobe Systems Incorporated. All rights reserved.

=cut


