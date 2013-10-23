# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2::Types::operationresult - Define Codex type data.

=head1 DESCRIPTION

See POD from Adobe::Codex2.pm for details on using the Codex modules.

=head2 XML

  <xs:element name="operationresult" type="xs:boolean"/>

=head1 SUBROUTINES/METHODS

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2::Types::operationresult;

require 5.8.0;
use strict;
use warnings;

use base qw(Exporter);
our $VERSION     = "0.2.0";
our @ISA         = qw();
our @EXPORT      = qw();
our @EXPORT_OK   = qw(&Data);
our %EXPORT_TAGS = ();

# Get Perl modules.
use XML::Parser;

# Get Codex modules.
# None.

# -------------------------------------------------------------------------
# Module variables.
# -------------------------------------------------------------------------
my $CurrentElement;
my $operationresult;

# -------------------------------------------------------------------------
# Data
# -------------------------------------------------------------------------
sub Data
{
    # Get arguments.
    my $XML = shift;

    # Check arguments.
    # TODO
    
    # Reset module variables.
    undef $operationresult;

    # Set up XML::Parser.
    my $XMLParser = XML::Parser->new(Handlers => {Start => \&operationresultStartHandler,
                                                  End   => \&operationresultEndHandler,
                                                  Char  => \&operationresultCharHandler});

    # Try to parse the XML.
    eval { $XMLParser->parse($XML); };
    
    # Handle parsing error.
    # TODO
    
    # Return results.
    return $operationresult;
}

# -------------------------------------------------------------------------
# Helper routines for XML::Parser.
# -------------------------------------------------------------------------

# TODO Make sure there is only one <operationresult> element.
sub operationresultStartHandler
{
    # Get arguments.
    my $Expat   = shift;
    my $Element = shift;
    my %Attr    = @_;
    
    # Save current element.
    $CurrentElement = $Element;

    # Return something.
    return 1;
}

sub operationresultEndHandler
{
    # Get arguments.
    my $Expat   = shift;
    my $Element = shift;

    # Check element type.
    undef $CurrentElement;

    # Return something.
    return 1;
}

sub operationresultCharHandler
{
    # Get arguments.
    my $Expat    = shift;
    my $Contents = shift;

    # Check element type.
    $operationresult = $Contents if ($CurrentElement eq "operationresult");

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


