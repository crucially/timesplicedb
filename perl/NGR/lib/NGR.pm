
package NGR;
use strict;
use version;

use NGR::C qw();

our $VERSION = '0.01';


sub new {
    my $class = shift;
    my %options = @_;

    my $self = bless {}, $class;
    my $names = [$options{name} || ""];
    my $flags = [$options{flags} || 0];
    my $columns = 0;
    foreach my $column (keys %{$options{columns}}) {
	$columns++;
	$names->[$columns] = $column;
	$flags->[$columns] = $options{columns}->{$column};
    }

    for(0..$options{columns}) {
	
    }

    
    if($options{create}) {
	$self->{ctx} = NGR::C::create( $options{filename},
				       $options{create_time},
				       $options{resolution},
				       $columns,
				       $names,
				       $flags,
	    );
    } else {
	$self->{ctx} = NGR::C::open( $options{filename} );
    }

    return $self;

}

sub insert {
    my $self = shift;
    my %options = @_;

    NGR::C::insert( $self->{ctx},
		    $options{column},
		    $options{timestamp},
		    $options{value} );
    
}

sub info {
    my $self = shift;
    return {
	created      => $self->created,
	resolution   => $self->resolution,
	version      => $self->version,
	columns      => $self->columns,
	rows         => $self->rows,
	last_updated => $self->last_updated,
    };
}


sub created {
    my $self = shift;
    return NGR::C::metric_created($self->{ctx});
}

sub resolution {
    my $self = shift;
    return NGR::C::metric_resolution($self->{ctx});
}

sub version {
    my $self = shift;
    return NGR::C::metric_version($self->{ctx});
}

sub columns {
    my $self = shift;
    return NGR::C::metric_columns($self->{ctx});
}


sub rows {
    my $self = shift;
    return NGR::C::last_row_idx($self->{ctx}, 0) + 1;
}

sub last_row_idx {
    my $self = shift;
    return NGR::C::last_row_idx($self->{ctx}, 0);
}

sub cell {
    my $self = shift;
    my %options = @_;
    return NGR::C::cell($self->{ctx}, $options{row}, $options{column});
}

sub last_updated {
    my $self = shift;
    return ($self->created + ($self->last_row_idx(column => 0) * $self->resolution));
}
sub name {
    my $self = shift;
    return NGR::C::metric_name($self->{ctx});
}

sub meta {
    my $self = shift;
    return NGR::C::metric_meta($self->{ctx});
}

sub timespan {
    my $self = shift;
    my %options = @_;
    return NGR::Range->new(NGR::C::timespan($self->{ctx}, $options{start}, $options{end}));
}


package NGR::Range;

use strict;
use warnings;

sub new {
    my $class = shift;
    my $self = bless {};
    $self->{ctx} = shift;
    $self->{iter} = 0;
    return $self;
}

sub rows {
    my $self = shift;
    NGR::C::range_rows($self->{ctx});
}

sub aggregate {
    my $self = shift;
    my %options = @_; 
    
    return NGR::Range->new(NGR::C::aggregate($self->{ctx}, $options{interval}, 0));
}

sub cell {
    my $self = shift;
    my %options = @_;

    return {
	row    => $options{row},
	time   => 0,  # XXX calculate the time offset
	value  => NGR::C::range_row_value($self->{ctx}, $options{column}, $options{row}),
	avg    => NGR::C::range_row_avg($self->{ctx}, $options{column}, $options{row}),
	min    => NGR::C::range_row_min($self->{ctx}, $options{column}, $options{row}),
	max    => NGR::C::range_row_max($self->{ctx}, $options{column}, $options{row}),
	stddev => NGR::C::range_row_stddev($self->{ctx}, $options{column}, $options{row}),
	rows_averaged => NGR::C::range_row_rows_averaged($self->{ctx}, $options{column}, $options{row}),
    };
}

1;
