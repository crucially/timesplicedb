

use Test::More tests => 27;
BEGIN { use_ok('TSDB') };

use strict;

my $ct = 1258091219;
my $it = $ct;
my $tsdb = TSDB->new('columntest.tsdb',
    clobber     => 1,
    resolution  => 60,
    columns     => { col1 => 1, col2 => 1},
    name  => "test database",
    create_time => $ct);

my $col0 = 1;
my $col1 = 9;

foreach(1..9) {
    $tsdb->insert(0 => $col0++, $it);
    
    $tsdb->insert('col2' => $col1--, $it);
    
    $it += 60;
}


is($tsdb->name, "test database", "");

is_deeply($tsdb->meta, { "col1" => 1, "col2" => 1 }, "");

is($tsdb->columns, 2);
is($tsdb->rows, 9);

is($tsdb->cell(0, 0),      1, "first column is number 1");
is($tsdb->cell(0, 1),      9,  "second column first cell");
is($tsdb->cell(1, 'col1'), 2, "first column second cell");
is($tsdb->cell(1, 'col2'), 8, "second column second cell");


my $range = $tsdb->timespan($ct => $it - 60);

isa_ok($range, "TSDB::Range");
is($range->rows, 9);

is($range->cell(0, 0)->{value}, 1);
is($range->cell(0, 1)->{value}, 9);

is($range->cell(1, 'col1')->{value}, 2);
is($range->cell(1, 'col2')->{value}, 8);

my $agg = $range->aggregate(interval => 120);

isa_ok($range, "TSDB::Range");
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
    unlink("columntest.tsdb");
}
