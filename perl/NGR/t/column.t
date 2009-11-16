

use Test::More tests => 25;
BEGIN { use_ok('NGR') };

use strict;

my $ct = 1258091219;
my $it = $ct;
my $ngr = NGR->new(
    create      => 1,
    filename    => 'columntest.ngrd',
    resolution  => 60,
    columns     => { col1 => 1, col2 => 1},
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
is($ngr->rows, 9);

is($ngr->cell(column => 0, row => 0), 1, "first column is number 1");
is($ngr->cell(column => 1, row => 0), 9, "second column first cell");
is($ngr->cell(column => 0, row => 1), 2, "first column second cell");
is($ngr->cell(column => 1, row => 1), 8, "second column second cell");


my $range = $ngr->timespan(start  => $ct,
			   end    => $it - 60,
			  );

isa_ok($range, "NGR::Range");
is($range->rows, 9);

is($range->cell(column => 0, row => 0)->{value}, 1);
is($range->cell(column => 1, row => 0)->{value}, 9);

is($range->cell(column => 0, row => 1)->{value}, 2);
is($range->cell(column => 1, row => 1)->{value}, 8);

my $agg = $range->aggregate(interval => 120);

isa_ok($range, "NGR::Range");
is($agg->rows, 5);

{
  my $cell = $agg->cell(column => 0, row => 0);
  is($cell->{stddev}, 0.5, "stddev");
  is($cell->{min}, 1, "lowest we have seen");
  is($cell->{max}, 2, "highest we have seen");
  is($cell->{avg}, 1.5 , "between 1 and 2");
  is($cell->{rows_averaged}, 2, "this bucket should have two entries!");
}


{
  my $cell = $agg->cell(column => 1, row => 0);
  is($cell->{stddev}, 0.5, "stddev");
  is($cell->{min}, 8, "lowest we have seen");
  is($cell->{max}, 9, "highest we have seen");
  is($cell->{avg}, 8.5 , "between 1 and 2");
  is($cell->{rows_averaged}, 2, "this bucket should have two entries!");
}



END {
    unlink("columntest.ngrd");
}
