#!/usr/bin/perl

use strict;
use warnings;
use List::Util;

use lib '.';
use NGR;


my $ngr = NGR->new();

$ngr->open("host","metric",(time-(3600*24*365)));
print scalar gmtime($ngr->created_time);
print "\n";
print $ngr->created_time;
print "\n";

exit;
$ngr->insert_entry(time(), int(rand(100)));

print $ngr->last_entry;
print "\n";



my $entries = $ngr->entries;
print "We have $entries entries in the db\n";
use Time::HiRes qw(time);
my $values = $ngr->range(0,$entries);
my $st = time();
my $foo = $ngr->aggregate_avg(2628000, $values); 
print (time() - $st);
print "\n";
print Dumper($foo);
#print scalar(@$values);
#print "\n";
use Data::Dumper;

#print Dumper($values);
#my $timespan = $ngr->timespan(time-180, time);
#my $timespan = $ngr->timespan(time-360, time-180);
#print Dumper($timespan);


#print Dumper($timespan);

