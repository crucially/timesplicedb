package TSDB;

use strict;
use version;
use Carp qw(croak);

use TSDB::C qw();

our $VERSION = '0.01';

=head1 NAME

TSDB - a timeslice database

=head1 SYNOPSIS

    use TSDB;
    my $tsdb = TSDB->new( $filename, columns => [ 'response_time',  'failure_rate'] );
    $tsdb->insert('response_time' => $value);


=head1 METHODS

=cut


=head2 new <filename> [option[s]]

=over 4

=item overwrite 

If true then will create a new database, no matter what.

Otherwise the default behaviour is to create a new database if none 
exists and open any existing ones.

=item name

A name for the database.

Defaults to the filename without file extension.

=item flags 

A bit pack of flags for the database.

=item resolution 

Takes a number, in milliseconds, of the interval between rows.

Defaults to ???

=item create_time

Takes a Unix time stamp which allows you to set the creation time of the db.

Defaults to the current time. 

=item columns <required>

The column definitions. Can be in multiple formats:

    # The simplest form - a single column with a name 
    columns => $name;

    # Second simplest form - multiple named columns
    columns => [$name1, $name2, $name3];

    # Third form - multiple columns with flags
    columns => { $name1 => $flag1, $name2 => $flag2, $name3 => $flag3 };

=back

=cut

sub new {
    my $class   = shift;
    my $file    = shift || croak "You must pass in a filename";
    my %options = @_;


    my $open    = 1;
    if ( -f $file ) {
        if ($options{clobber}) {
            unlink($file);
            $open = 0;
        }
    } else { 
        $open = 0;
    }

    my $self    = bless {}, $class;
    my ($name)  = ($options{name}) || ($file =~ m!([^./]+)(?:\..*)?$!);  

    # TODO make this nicer than a bit pack
    my $flags = [$options{flags} || 0];

    my @col_names;
    my @col_flags;

    my $i = 0;
    if ('HASH' eq ref($options{columns})) {
        while (my ($k,$v) = each %{$options{columns}}) {
        push @col_names, $k;
        push @col_flags, $v;
        $self->{_columns}->{$k} = $i++;
        }
    } elsif ('ARRAY' eq ref($options{columns})) {
        foreach my $k (@{$options{columns}}) {
        push @col_names, $k;
        push @col_flags, 0;
        $self->{_columns}->{$k} = $i++;
        }
    } elsif (defined $options{columns}) {
        push @col_names, $options{columns};
        push @col_flags, 0;
        $self->{_columns}->{$options{columns}} = 0;
    } elsif (!$open) {
        croak "You must pass in a columns definition";
    }


    $self->{ctx} = ($open) ? TSDB::C::open($file) :    
                             TSDB::C::create($file,
                                $options{create_time} || 0,
                                $options{resolution}  || 0,
                                scalar(@col_names),
                                $name,
                                $flags || 0,
                                \@col_names,
                                \@col_flags,
                         );

    return $self;

}

=head2 info 

Get info about the database.

=cut
sub info {
    my $self = shift;
    return {
        name         => $self->name,
        created      => $self->created,
        resolution   => $self->resolution,
        version      => $self->version,
        columns      => $self->columns,
        rows         => $self->rows,
        last_updated => $self->last_updated,
        meta         => $self->meta,
    };
}


=head2 name

Get the name of the database

=cut
sub name {
    my $self = shift;
    return TSDB::C::metric_name($self->{ctx});
}

=head2 created

Get when the database was created.

=cut
sub created {
    my $self = shift;
    return TSDB::C::metric_created($self->{ctx});
}

=head2 resolution

Get the resolution of the database

=cut
sub resolution {
    my $self = shift;
    return TSDB::C::metric_resolution($self->{ctx});
}

=head2 version

Get the database version

=cut
sub version {
    my $self = shift;
    return TSDB::C::metric_version($self->{ctx});
}

=head2 columns

Get the number of columns.

=cut
sub columns {
    my $self = shift;
    return TSDB::C::metric_columns($self->{ctx});
}


=head2 rows

Get the number of rows

=cut
sub rows {
    my $self = shift;
    return TSDB::C::last_row_idx($self->{ctx}, 0) + 1;
}

=head2 last_row_idx

Get the index of the last row

=cut
sub last_row_idx {
    my $self = shift;
    return TSDB::C::last_row_idx($self->{ctx}, 0);
}

=head2 last_updated

Get the last time a row was inserted as a Unix timestamp.

=cut
sub last_updated {
    my $self = shift;
    return ($self->created + ($self->last_row_idx(column => 0) * $self->resolution));
}

=head2 meta

Get some meta data about the database.

=cut
sub meta {
    my $self = shift;
    return TSDB::C::metric_meta($self->{ctx});
}


=head2 insert <column> => <value> [timestamp]

Insert the I<value> into I<column>.

=cut
sub insert {
    my $self = shift;
    my ($column, $value, $ts) = @_;
    $ts ||= 0;

    return TSDB::C::insert( $self->{ctx}, TSDB::_fix_col($self, $column), $ts, $value );
}


=head2 cell <row> <column>

Get the value of a cell.

=cut
sub cell {
    my $self = shift;
    my ($row, $column) = @_;

    return TSDB::C::cell($self->{ctx}, $row, TSDB::_fix_col($self,$column));
}

=head2 range <start> <end>

Get all the values between the I<start> row and I<end> row.

Returns a C<TSDB::Range>

=cut
sub range {
    my $self = shift;
    my ($start, $end) = @_;
    return TSDB::Range->new(TSDB::C::range($self->{ctx}, $start, $end), _columns => $self->{_columns});
}

=head2 timespan <start> <end>

Get all values between the unix time stamps I<start> and I<end>.

Returns a C<TSDB::Range>

=cut
sub timespan {
    my $self = shift;
    my ($start, $end) = @_;
    return TSDB::Range->new(TSDB::C::timespan($self->{ctx}, $start, $end), _columns => $self->{_columns});
}

# turn a column name into an number
sub _fix_col {
    my $self = shift;
    my $col  = shift;
    {
    no warnings;
    return $col if(int($col) eq $col);
    }
    return $self->{_columns}->{$col} if (exists $self->{_columns}->{$col});
    die "Unknown column '$col'";
}

package TSDB::Range;

use strict;
use warnings;

=head1 NAME

TSDB::Range - represent a range of values in TSDB

=head1 SYNOPSIS
    
    my $tsdb  = TSDB->new($filename);
    my $cols  = $tsdb->columns;
    my $range = $tsdb->timespan($start, $end);
    foreach my $row (0 .. $range->rows) {
        foreach my $col (0..$cols) {
            print Dumper ($range->cell($row, $col));
        }   
    }


=head1 METHODS

=cut

=head2 new 

This should only be called internally.

=cut
sub new {
    my $class = shift;
    my $ctx   = shift;
    my %opts  = @_;
    my $self  = bless \%opts, $class;
    $self->{ctx}  = $ctx;
    $self->{iter} = 0;
    return $self;
}

=head2 rows

The number of rows in this range

=cut
sub rows {
    my $self = shift;
    TSDB::C::range_rows($self->{ctx});
}

=head2 aggregate <interval> 

The aggregate of the rows over the interval.

Returns an C<TSDB::Range>

=cut
sub aggregate {
    my $self     = shift;
    my $interval = shift;

    return TSDB::Range->new(TSDB::C::aggregate($self->{ctx}, $interval, 0, 1), _columns => $self->{_columns});
}

=head2 cell <row> <column> 

Returns information about a cell in a columns.

=cut
sub cell {
    my $self    = shift;
    my $row     = shift;
    my $column  = TSDB::_fix_col($self, shift);
    my %options = @_;

    return {
        row    => $row,
        time   => 0,  # XXX calculate the time offset
        value  => TSDB::C::range_row_value($self->{ctx},  $column, $row),
        avg    => TSDB::C::range_row_avg($self->{ctx},    $column, $row),
        min    => TSDB::C::range_row_min($self->{ctx},    $column, $row),
        max    => TSDB::C::range_row_max($self->{ctx},    $column, $row),
        stddev => TSDB::C::range_row_stddev($self->{ctx}, $column, $row),
        rows_averaged => TSDB::C::range_row_rows_averaged($self->{ctx}, $column, $row),
    };
}

1;
