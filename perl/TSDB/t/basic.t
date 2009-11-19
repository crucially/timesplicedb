#!/usr/bin/perl


# change 'tests => 2' to 'tests => last_test_to_print';

use Test::More tests => 2;
BEGIN { use_ok('TSDB::C') };
unlink("test.data");

my $ts = time-3600;

my $tsdb = TSDB::C::create("test.data", $ts, 60, 1,"foo", 0, ["bar"],[0]);

for(1..60) {
  TSDB::C::insert($tsdb, 0, ($ts+($_*60)), int rand(100));
}
ok(1);

my $range = TSDB::C::timespan($tsdb, $ts, time);

my $agg = TSDB::C::aggregate($range, 600, 0);





