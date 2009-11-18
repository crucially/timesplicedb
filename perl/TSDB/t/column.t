

use Test::More tests => 27;
BEGIN { use_ok('NGR') };

use strict;

my $ct = 1258091219;
my $it = $ct;
my $ngr = NGR->new('columntest.ngrd',
    clobber     => 1,
    resolution  => 60,
    columns     => { col1 => 1, col2 => 1},
    name  => "test database",
    create_time => $ct);

my $col0 = 1;
my $col1 = 9;

foreach(1..9) {
    $ngr->insert(0 => $col0++, $it);
    
    $ngr->insert('col2' => $col1--, $it);
    
    $it += 60;
}


is($ngr->name, "test database", "");

is_deeply($ngr->meta, { "col1" => 1, "col2" => 1 }, "");

is($ngr->columns, 2);
is($ngr->rows, 9);

is($ngr->cell(0, 0),      1, "first column is number 1");
is($ngr->cell(0, 1),      9,  "second column first cell");
is($ngr->cell(1, 'col1'), 2, "first column second cell");
is($ngr->cell(1, 'col2'), 8, "second column second cell");


my $range = $ngr->timespan($ct => $it - 60);

isa_ok($range, "NGR::Range");
is($range->rows, 9);

is($range->cell(0, 0)->{value}, 1);
is($range->cell(0, 1)->{value}, 9);

is($range->cell(1, 'col1')->{value}, 2);
is($range->cell(1, 'col2')->{value}, 8);

my $agg = $range->aggregate(interval => 120);

isa_ok($range, "NGR::Range");
is($agg->rows, 5);

{
  my $cell = $agg->cell(0, 0);
  is($cell->{stddev}, 0.5, "stddev");
  is($cell->{min}, 1, "lowest we have seen");
  is($cell->{max}, 2, "highest we have seen");
  is($cell->{avg}, 1.5 , "between 1 and 2");
  is($cell->{rows_averaged}, 2, "this bucket should have two entries!");
}


{
  my $cell = $agg->cell(0, 'col2');
  is($cell->{stddev}, 0.5, "stddev");
  is($cell->{min}, 8, "lowest we have seen");
  is($cell->{max}, 9, "highest we have seen");
  is($cell->{avg}, 8.5 , "between 1 and 2");
  is($cell->{rows_averaged}, 2, "this bucket should have two entries!");
}



END {
    unlink("columntest.ngrd");
}
