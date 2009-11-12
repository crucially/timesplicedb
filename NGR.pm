#!/usr/bin/perl


use strict;
use warnings;
package NGR;
use Config;

sub new {
    return bless {};
}


sub open {
    my $self = shift;
    my $host = shift;
    my $metric = shift;
    my $ts = shift || time;
    my $path = "data/$host/$metric.data";
    my $fh;

    unless(-d "data/$host") {
	mkdir("data/$host");
    }
    if(-e $path ) {
	open($fh, "+<data/$host/$metric.data") || die;	
	seek($fh, 0,0);
	my $buffer;
	read($fh, $buffer,4);
	$self->{width} = unpack("L",$buffer);
	if($self->{width} == 8) {
	    die "Cannot open 64bit file on 32bit perl" unless $Config{use64bitint};
	    print "Byte width $self->{width}\n";
	    $self->{pack} = "Q";
	} elsif($self->{width} == 4) {
	    $self->{pack} = "L";
	    print "Byte width $self->{width}\n";
	} else {
	    die "Unknown byte width $self->{width}\n";
	}

    } else {
	open($fh, "+>data/$host/$metric.data") || die "$!";
	if($Config{use64bitint}) {
	    $self->{pack} = "Q";
	    print $fh pack("L",8);
	    $self->{width} = 8;
	} else {
	    $self->{pack} = "L";
	    print $fh pack("L",4);
	    $self->{width} = 4;
	}
	print $fh pack($self->{pack},$ts);
    }
    $self->{base} = 4 + $self->{width};
    $self->{fh} = $fh;
}

sub created_time {
    my $self = shift;
    my $buffer;
    seek($self->{fh}, 4,0);
    my $len = read($self->{fh}, $buffer, $self->{width}, 0);
    return unpack($self->{pack}, $buffer);
}

sub insert_entry {
    my $self = shift;
    my $ts = shift;
    my $value = shift;
    my $delta_sec = $ts - $self->created_time;
    my $delta_min = $delta_sec/60;
    my $entries_offset = int($delta_min) || 1;
    my $offset = ($self->{base} + ($entries_offset * $self->{width}));
#    print "storing $value at $offset\n";
    seek($self->{fh}, $offset, 0);
    print {$self->{fh}} pack($self->{pack}, $value);
}

sub last_entry {
    my $self = shift;
    my $buffer;
    seek($self->{fh}, 0-$self->{width},2);
    print tell($self->{fh})."\n";
    my $len = read($self->{fh}, $buffer, $self->{width}, 0);
    return unpack($self->{pack}, $buffer);
}

sub range {
    my $self = shift;
    my $start = shift;
    my $end = shift;
    my $items = $end - $start;

    seek($self->{fh}, ($self->{base} + ($start * $self->{width})), 0);
    my $buffer = shift;
    my $wanted_len = $items * $self->{width};
    my $len = read($self->{fh}, $buffer, $wanted_len);
    warn "wanted $wanted_len, only read $len" unless($wanted_len == $len);
    return [unpack("$self->{pack}$items", $buffer)];
}

sub entries {
    my $self = shift;
    seek($self->{fh}, 0, 2);
    return ((tell($self->{fh}) - $self->{base}) / $self->{width});
}

sub timespan {
    my $self = shift;
    my $start_time = shift;
    my $stop_time = shift;
    my $created_time = $self->created_time;
    my $start_offset = int(($start_time - $created_time + 60) / 60);
    my $stop_offset = int(($stop_time - $created_time + 60) / 60);
    return $self->range($start_offset, $stop_offset);
}

sub aggregate_avg {
    my $self = shift;
    my $interval = shift;
    my $range = shift;
    my $entries_interval = $interval / 60;
    my $entries_seen = 0;
    my $sum = 0;
    my $aggregate = [];
    foreach my $entry (@$range) {
	if($entries_interval == $entries_seen++) {
	    push @$aggregate, $sum/$entries_seen;
	    $entries_seen = 0;
	    $sum = 0;
	}
	$sum += $entry;
    }
    return $aggregate;
}

sub aggregate_avg {
    my $self = shift;
    my $interval = shift;
    my $range = shift;
    my $entries_interval = $interval / 60;
    my $entries_seen = 0;
    my $sum = 0;
    my $aggregate = [];
    foreach my $entry (@$range) {
	if($entries_interval == $entries_seen++) {
	    push @$aggregate, $sum/$entries_seen;
	    $entries_seen = 0;
	    $sum = 0;
	}
	$sum += $entry;
    }
    return $aggregate;
}

1;
