# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2::Types::source - Define Codex type data.

=head1 DESCRIPTION

See POD from Adobe::Codex2.pm for details on using the Codex modules.

=head2 XML

  <xs:element name="source">
    <xs:complexType>
      <xs:attribute name="path" type="xs:string" use="optional"/>
      <xs:attribute name="protocol" type="xs:string" use="optional"/>
      <xs:attribute name="query" type="xs:string" use="optional"/>
      <xs:attribute name="server" type="xs:string" use="optional"/>
    </xs:complexType>
  </xs:element>

=head1 SUBROUTINES/METHODS

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2::Types::source;

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
# None.

# Get Codex modules.
# None.

# -------------------------------------------------------------------------
# Data
# -------------------------------------------------------------------------
sub Data
{
    # Get arguments.
    my $Data = shift;

    # Check arguments.
    # TODO
    
    # Check data.
    # TODO

    # Storage for results.
    my %source;

    # Process data.
    $source{'path'}     = $Data->{'path'}     if (exists $Data->{'path'});
    $source{'protocol'} = $Data->{'protocol'} if (exists $Data->{'protocol'});
    $source{'query'}    = $Data->{'query'}    if (exists $Data->{'query'});
    $source{'server'}   = $Data->{'server'}   if (exists $Data->{'server'});
    
    # Return results.
    return %source;
}

1;
__END__

=pod

=head1 AUTHOR

    Dave Foglesong (fogleson@adobe.com)

=head1 COPYRIGHT

    Copyright 2008 Adobe Systems Incorporated. All rights reserved.

=cut

