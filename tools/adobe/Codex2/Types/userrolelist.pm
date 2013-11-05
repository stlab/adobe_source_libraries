# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2::Types::userrolelist - Define Codex type data.

=head1 DESCRIPTION

See POD from Adobe::Codex2.pm for details on using the Codex modules.

=head2 XML

  <xs:element name="userrolelist">
    <xs:complexType>
      <xs:sequence>
        <xs:element maxOccurs="unbounded" minOccurs="0" ref="schemas:userrole"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>  

=head1 SUBROUTINES/METHODS

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2::Types::userrolelist;

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
use Adobe::Codex2::Types::userrole;

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
    my @userrolelist;

    # Process data.
    if (exists $Data->{'userrole'})
    {
        # Single item.
        if (ref($Data->{'userrole'}) eq "HASH")
        {
            my %userrole = Adobe::Codex2::Types::userrole::Data($Data->{'userrole'});
            push(@userrolelist, \%userrole);
        }
        # Multiple items.
        elsif (ref($Data->{'userrole'}) eq "ARRAY")
        {
            foreach my $userroleData (@{$Data->{'userrole'}})
            {
                my %userrole = Adobe::Codex2::Types::userrole::Data($userroleData);
                push(@userrolelist, \%userrole);
            }
        }
    }

    # Return results.
    return @userrolelist;
}

1;
__END__

=pod

=head1 AUTHOR

    Dave Foglesong (fogleson@adobe.com)

=head1 COPYRIGHT

    Copyright 2008 Adobe Systems Incorporated. All rights reserved.

=cut


