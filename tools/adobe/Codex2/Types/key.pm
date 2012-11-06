# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2::Types::key - Define Codex type data.

=head1 DESCRIPTION

See POD from Adobe::Codex2.pm for details on using the Codex modules.

=head2 XML

  <xs:element name="key" type="xs:string"/>

=head1 SUBROUTINES/METHODS

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2::Types::key;

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
    my $key = "";
    
    # Process data.
    $key = $Data if (defined $Data);

    # Return results.
    return $key;
}

1;
__END__

=pod

=head1 AUTHOR

    Dave Foglesong (fogleson@adobe.com)

=head1 COPYRIGHT

    Copyright 2008 Adobe Systems Incorporated. All rights reserved.

=cut

