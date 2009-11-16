

use Test::More tests => 13;
BEGIN { use_ok('NGR') };

use strict;

my $ct = 1258091219;
my $it = $ct;
my $ngr = NGR->new(
    create      => 1,
    filename    => 'columntest.ngrd',
    resolution  => 60,
    columns     => 2,
    create_time => $ct);

my $col0 = 1;
my $col1 = 9;

foreach(1..9) {
    $ngr->insert(column => 0,
		 timestamp => $it,
		 value => $col0++,
	);
    
    $ngr->insert(column => 1,
		 timestamp => $it,
		 value => $col1--,
	);
    
    
    $it += 60;
}


is($ngr->columns, 2);
is($ngr->items, 9);

is($ngr->entry(column => 0, idx => 0), 1, "first column is number 1");
is($ngr->entry(column => 1, idx => 0), 9, "second column first entry");
is($ngr->entry(column => 0, idx => 1), 2, "first column second entry");
is($ngr->entry(column => 1, idx => 1), 8, "second column second entry");


my $range = $ngr->timespan(start  => $ct,
			   end    => $it - 60,
			  );

isa_ok($range, "NGR::Range");
is($range->items, 9);

is($range->entry(column => 0, idx => 0)->{value}, 1);
is($range->entry(column => 1, idx => 0)->{value}, 9);

is($range->entry(column => 0, idx => 1)->{value}, 2);
is($range->entry(column => 1, idx => 1)->{value}, 8);


END {
    unlink("columntest.ngrd");
}
