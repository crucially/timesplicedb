

use Test::More tests => 17;
BEGIN { use_ok('NGR') };

use strict;

my $ct = 1258091219;
my $it = $ct;
my $ngr = NGR->new(
    create      => 1,
    filename    => 'aggtest.ngrd',
    resolution  => 60,
    columns     => 1,
    create_time => $ct);

foreach(1..9) {
  $ngr->insert(column => 0,
	       timestamp => $it,
	       value => $_,
	       );
  $it += 60;
}


is($ngr->items, 9);

my $range = $ngr->timespan(column => 0,
			   start  => $ct,
			   end    => $it - 60,
			  );

isa_ok($range, "NGR::Range");
is($range->items, 9);

my $agg = $range->aggregate(interval => 120);
isa_ok($range, "NGR::Range");
is($agg->items, 5);



END { unlink("aggtest.ngrd") }
