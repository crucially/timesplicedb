
package NGR;
use strict;
use version;

use NGR::C qw();

our $VERSION = '0.01';


sub new {
    my $class = shift;
    my %options = @_;

    my $self = bless {}, $class;

    
    if($options{create}) {
	$self->{ctx} = NGR::C::create( $options{filename},
				       $options{create_time},
				       $options{resolution},
				       $options{columns} );
    } else {
	$self->{ctx} = NGR::C::create( $options{filename} );
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
	items        => $self->items,
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


sub items {
    my $self = shift;
    return NGR::C::last_entry_idx($self->{ctx}, 0) + 1;
}

sub last_entry_idx {
    my $self = shift;
    return NGR::C::last_entry_idx($self->{ctx}, 0);
}

sub entry {
    my $self = shift;
    my %options = @_;
    return NGR::C::entry($self->{ctx}, $options{column}, $options{idx});
}

sub last_updated {
    my $self = shift;
    return ($self->created + ($self->last_entry_idx(column => 0) * $self->resolution));
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

sub items {
    my $self = shift;
    NGR::C::range_items($self->{ctx});
}

sub aggregate {
    my $self = shift;
    my %options = @_; 
    
    return NGR::Range->new(NGR::C::aggregate($self->{ctx}, $options{interval}, 0));
}

sub entry {
    my $self = shift;
    my %options = @_;

    return {
	idx    => $options{idx},
	time   => 0,  # XXX calculate the time offset
	value  => NGR::C::range_entry_value($self->{ctx}, $options{column}, $options{idx}),
	avg    => NGR::C::range_entry_avg($self->{ctx}, $options{column}, $options{idx}),
	min    => NGR::C::range_entry_min($self->{ctx}, $options{column}, $options{idx}),
	max    => NGR::C::range_entry_max($self->{ctx}, $options{column}, $options{idx}),
	stddev => NGR::C::range_entry_stddev($self->{ctx}, $options{column}, $options{idx}),
	items_averaged => NGR::C::range_entry_items_averaged($self->{ctx}, $options{column}, $options{idx}),
    };
}

1;
