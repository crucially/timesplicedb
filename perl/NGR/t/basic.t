#!/usr/bin/perl


# change 'tests => 2' to 'tests => last_test_to_print';

use Test::More tests => 2;
BEGIN { use_ok('NGR::C') };

my $ts = time-3600;

my $ngr = NGR::C::create("test.data", $ts, 60, 1);

for(1..60) {
  NGR::C::insert($ngr, 0, ($ts+($_*60)), int rand(100));
}
ok(1);

my $range = NGR::C::timespan($ngr, $ts, time);

my $agg = NGR::C::aggregate($range, 600, 0);



unlink("test.data");

