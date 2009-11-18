#!/usr/bin/perl


# change 'tests => 2' to 'tests => last_test_to_print';
unlink("64bit.tsdb");
use Test::More tests => 4;

BEGIN { use_ok('TSDB::C') };

my $ts = time-3600;

my $tsdb = TSDB::C::create("64bit.tsdb", $ts, 60, 1,"foo", 0, ["bar"],[0]);
TSDB::C::insert($tsdb, 0, $ts, 5294967395);
TSDB::C::insert($tsdb, 0, $ts+60, 3294967395);
TSDB::C::insert($tsdb, 0, $ts+120, "18446744073709551615");
is(TSDB::C::cell($tsdb, 0, 0), 5294967395);
is(TSDB::C::cell($tsdb, 1, 0), 3294967395);
{ local $TODO = "need to fake it using bigints";
  is(TSDB::C::cell($tsdb, 2, 0), "18446744073709551615");
}



