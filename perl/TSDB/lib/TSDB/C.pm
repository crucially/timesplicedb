package TSDB::C;

use 5.008;
use strict;
use warnings;
use Carp;

require Exporter;
use AutoLoader;

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use TSDB ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	TSDB_COUNTER
	TSDB_GAUGE
	aggregate
	create
	entry
	insert
	last_entry_idx
	open
	range
	range_free
	timespan
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	TSDB_COUNTER
	TSDB_GAUGE
);

our $VERSION = '0.01';

sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.

    my $constname;
    our $AUTOLOAD;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    croak "&TSDB::constant not defined" if $constname eq 'constant';
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
XSLoader::load('TSDB', $VERSION);

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

TSDB - Perl extension for blah blah blah

=head1 SYNOPSIS

  use TSDB;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for TSDB, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head2 EXPORT

None by default.

=head2 Exportable constants

  TSDB_COUNTER
  TSDB_GAUGE

=head2 Exportable functions

  struct TSDB_range_t * TSDB_aggregate (struct TSDB_range_t *range, int interval, int data_type)
  struct TSDB_metric_t * TSDB_create(char *filename, time_t create_time, int resolution, int columns)
  int TSDB_entry (struct TSDB_metric_t *obj, int column, int idx)
  int TSDB_insert (struct TSDB_metric_t *obj, int column, time_t timestmp, int value)
  int TSDB_last_entry_idx (struct TSDB_metric_t *obj, int column)
  char * TSDB_make_path (char *collection, char *metric)
  struct TSDB_metric_t * TSDB_open(char *filename)
  struct TSDB_range_t * TSDB_range (struct TSDB_metric_t *obj, int column, int start, int end)
  void TSDB_range_free (struct TSDB_range_t *range)
  struct TSDB_range_t * TSDB_timespan (struct TSDB_metric_t *obj, int column, time_t start, time_t end)



=head1 SEE ALSO

Mention other useful documentation such as the documentation of
related modules or operating system documentation (such as man pages
in UNIX), or any relevant external documentation such as RFCs or
standards.

If you have a mailing list set up for your module, mention it here.

If you have a web site set up for your module, mention it here.

=head1 AUTHOR

Artur Bergman, E<lt>sky@crucially.netE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2009 by Artur Bergman

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.9 or,
at your option, any later version of Perl 5 you may have available.


=cut
