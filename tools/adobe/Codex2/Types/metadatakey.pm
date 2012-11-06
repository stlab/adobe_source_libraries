# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2::Types::metadatakey - Define Codex type data.

=head1 DESCRIPTION

See POD from Adobe::Codex2.pm for details on using the Codex modules.

=head2 XML

  <xs:element name="metadatakey">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element ref="schemas:id"/>
        <xs:element ref="schemas:key"/>
      </xs:sequence>
      <xs:attribute name="product" type="xs:string" use="optional"/>
      <xs:attribute name="version" type="xs:string" use="optional"/>
    </xs:complexType>
  </xs:element>

=head1 SUBROUTINES/METHODS

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2::Types::metadatakey;

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
use Adobe::Codex2::Types::id;
use Adobe::Codex2::Types::key;

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
    my %metadatakey;

    # Process data.
    $metadatakey{'product'} = $Data->{'product'} if (exists $Data->{'product'});
    $metadatakey{'version'} = $Data->{'version'} if (exists $Data->{'version'});
    
    my $id = Adobe::Codex2::Types::id::Data($Data->{'id'});
    $metadatakey{'id'} = $id;
    
    my $key = Adobe::Codex2::Types::id::Data($Data->{'key'});
    $metadatakey{'key'} = $key;
    
    # Return results.
    return %metadatakey;
}

1;
__END__

=pod

=head1 AUTHOR

    Dave Foglesong (fogleson@adobe.com)

=head1 COPYRIGHT

    Copyright 2008 Adobe Systems Incorporated. All rights reserved.

=cut

