#!/usr/bin/perl


# change 'tests => 2' to 'tests => last_test_to_print';

use Test::More tests => 2;
BEGIN { use_ok('TSDB::C') };

my $ts = time-3600;

my $ngr = TSDB::C::create("test.data", $ts, 60, 1,"foo", 0, ["bar"],[0]);

for(1..60) {
  TSDB::C::insert($ngr, 0, ($ts+($_*60)), int rand(100));
}
ok(1);

my $range = TSDB::C::timespan($ngr, $ts, time);

my $agg = TSDB::C::aggregate($range, 600, 0);



unlink("test.data");

