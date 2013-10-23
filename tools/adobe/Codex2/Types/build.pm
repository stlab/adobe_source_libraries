# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2::Types::build - Define Codex type data.

=head1 DESCRIPTION

See POD from Adobe::Codex2.pm for details on using the Codex modules.

=head2 XML

  <xs:element name="build">
    <xs:complexType>
      <xs:sequence maxOccurs="1" minOccurs="1">
        <xs:element minOccurs="0" ref="schemas:location"/>
        <xs:element minOccurs="0" ref="schemas:sources"/>
        <xs:element minOccurs="0" ref="schemas:notes"/>
        <xs:element minOccurs="0" ref="schemas:metadata"/>
      </xs:sequence>
      <xs:attribute name="build" type="xs:string" use="optional"/>
      <xs:attribute name="certlevel" type="xs:string" use="required"/>
      <xs:attribute name="compilertarget" type="xs:string" use="required"/>
      <xs:attribute name="date" type="xs:string" use="optional"/>
      <xs:attribute name="format" type="xs:string" use="required"/>
      <xs:attribute name="id" type="xs:integer" use="required"/>
      <xs:attribute name="language" type="xs:string" use="optional"/>
      <xs:attribute name="licensemodel" type="xs:string" use="required"/>
      <xs:attribute name="platform" type="xs:string" use="required"/>
      <xs:attribute name="product" type="xs:string" use="required"/>
      <xs:attribute name="status" type="xs:string" use="optional"/>
      <xs:attribute name="version" type="xs:string" use="required"/>
      <xs:attribute name="subproduct" type="xs:string" use="required"/>
    </xs:complexType>
  </xs:element>

=head1 SUBROUTINES/METHODS

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2::Types::build;

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
use Adobe::Codex2::Types::location;
use Adobe::Codex2::Types::sources;
use Adobe::Codex2::Types::notes;
use Adobe::Codex2::Types::metadata;

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
    my %build;

    # Process data.
    $build{'id'}             = $Data->{'id'};
    $build{'build'}          = $Data->{'build'} if (exists $Data->{'build'});
    $build{'certlevel'}      = $Data->{'certlevel'};
    $build{'compilertarget'} = $Data->{'compilertarget'};
    $build{'date'}           = $Data->{'date'} if (exists $Data->{'date'});
    $build{'format'}         = $Data->{'format'};
    $build{'language'}       = $Data->{'language'} if (exists $Data->{'language'});
    $build{'licensemodel'}   = $Data->{'licensemodel'};
    $build{'platform'}       = $Data->{'platform'};
    $build{'product'}        = $Data->{'product'};
    $build{'status'}         = $Data->{'status'} if (exists $Data->{'status'});
    $build{'version'}        = $Data->{'version'};
    $build{'subproduct'}     = $Data->{'subproduct'};
    
    my %location = Adobe::Codex2::Types::location::Data($Data->{'location'});
    $build{'location'} = \%location;
    
    my @sources = Adobe::Codex2::Types::sources::Data($Data->{'sources'});
    $build{'sources'} = \@sources;
    
    my @notes = Adobe::Codex2::Types::notes::Data($Data->{'notes'});
    $build{'notes'} = \@notes;

    my @metadata = Adobe::Codex2::Types::metadata::Data($Data->{'metadata'});
    $build{'metadata'} = \@metadata;

    # Return results.
    return %build;
}

1;
__END__

=pod

=head1 AUTHOR

    Dave Foglesong (fogleson@adobe.com)

=head1 COPYRIGHT

    Copyright 2008 Adobe Systems Incorporated. All rights reserved.

=cut

