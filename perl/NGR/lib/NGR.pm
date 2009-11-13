package NGR;

use 5.008009;
use strict;
use warnings;
use Carp;

require Exporter;
use AutoLoader;

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use NGR ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	NGR_COUNTER
	NGR_GAUGE
	NGR_aggregate
	NGR_create
	NGR_entry
	NGR_insert
	NGR_last_entry_idx
	NGR_make_path
	NGR_open
	NGR_range
	NGR_range_free
	NGR_timespan
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	NGR_COUNTER
	NGR_GAUGE
);

our $VERSION = '0.01';

sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.

    my $constname;
    our $AUTOLOAD;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    croak "&NGR::constant not defined" if $constname eq 'constant';
    my ($error, $val) = constant($constname);
    if ($error) { croak $error; }
    {
	no strict 'refs';
	# Fixed between 5.005_53 and 5.005_61
#XXX	if ($] >= 5.00561) {
#XXX	    *$AUTOLOAD = sub () { $val };
#XXX	}
#XXX	else {
	    *$AUTOLOAD = sub { $val };
#XXX	}
    }
    goto &$AUTOLOAD;
}

require XSLoader;
XSLoader::load('NGR', $VERSION);

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

NGR - Perl extension for blah blah blah

=head1 SYNOPSIS

  use NGR;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for NGR, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head2 EXPORT

None by default.

=head2 Exportable constants

  NGR_COUNTER
  NGR_GAUGE

=head2 Exportable functions

  struct NGR_range_t * NGR_aggregate (struct NGR_range_t *range, int interval, int data_type)
  struct NGR_metric_t * NGR_create(char *filename, time_t create_time, int resolution, int columns)
  int NGR_entry (struct NGR_metric_t *obj, int column, int idx)
  int NGR_insert (struct NGR_metric_t *obj, int column, time_t timestmp, int value)
  int NGR_last_entry_idx (struct NGR_metric_t *obj, int column)
  char * NGR_make_path (char *collection, char *metric)
  struct NGR_metric_t * NGR_open(char *filename)
  struct NGR_range_t * NGR_range (struct NGR_metric_t *obj, int column, int start, int end)
  void NGR_range_free (struct NGR_range_t *range)
  struct NGR_range_t * NGR_timespan (struct NGR_metric_t *obj, int column, time_t start, time_t end)



=head1 SEE ALSO

Mention other useful documentation such as the documentation of
related modules or operating system documentation (such as man pages
in UNIX), or any relevant external documentation such as RFCs or
standards.

If you have a mailing list set up for your module, mention it here.

If you have a web site set up for your module, mention it here.

=head1 AUTHOR

Artur Bergman, E<lt>sky@localE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2009 by Artur Bergman

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.9 or,
at your option, any later version of Perl 5 you may have available.


=cut
