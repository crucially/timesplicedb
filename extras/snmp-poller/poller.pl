#!/usr/bin/perl

use strict;

use warnings;

use strict;
use warnings;

use SNMP::Info;

use BigInt;
use bigint;

my $host = shift;

my $info = SNMP::Info->new(
    AutoSpecify => 1,
    Debug => 0,
    DestHost => $host,
    Community => 'UL9eicheDouS0ge4',
    Version => 2,
    MibDirs => ['/usr/local/netdisco/mibs'],
    BigInt => 1,
    ) || die "$!";


use TSDB;
my %metrics;

use Data::Dumper;
my %last_byte;
my $ts = time;
while(1) {

    my $time = time;
    my $interfaces = $info->interfaces;
    my $names      = $info->i_name;
    my $up         = $info->i_up;
    my $speed      = $info->i_speed_raw;
    my $in         = $info->i_octet_in64;
    my $out        = $info->i_octet_out64;
    print scalar gmtime() . "\n";
    my $time_diff = time - $ts;
    print "Run took $time_diff seconds\n";

    foreach my $iid (sort {$interfaces->{$a} cmp $interfaces->{$b} }keys %$interfaces) {

        my $port = $interfaces->{$iid};
        my $name = $names->{$iid};
        next unless ($up->{$iid} eq 'up');
	mkdir("data/$host");

        my $metric;
        unless($metric = $metrics{$port}) {
            $metric = $metrics{$port} = TSDB->new(
		"data/$host/$iid.tsdb",
                create      => 1,
                resolution  => 60,
                columns     => [ 'in_bytes', 
				 'out_bytes',
				 'in_packets',
				 'out_packets',
				 'err_in',
				 'err_out',
		],
                name  => "Network traffic $host $port ($name)",
                create_time => time());
        };
        $metric->insert(0, $in->{$iid});

        $metric->insert(1, $out->{$iid});
        print "Inserted $port $name $in->{$iid} <-> $out->{$iid})\n";

    }
    print "Sleeping " . (60 - $time_diff) . " seconds\n*****\n";
    sleep 60 - $time_diff;
    $ts = time;
    $info->update;
}
