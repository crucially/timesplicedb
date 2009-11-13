#!/usr/bin/perl

use strict;
use warnings;
use List::Util;

use lib '.';
use NGR;


my $ngr = NGR->new();

my $ts = (time-3600*24*365);
$ngr->open("host","metric_year_counter",$ts);
$ts = $ngr->created_time;
my $total = 60*24*365;
my $i = 0;
my $prev = 0;
for(0..$total) {
    $ngr->insert_entry($ts, $prev += rand(int(2)));
    $ts += 60;
}

