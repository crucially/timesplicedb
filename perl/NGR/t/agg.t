

use Test::More tests => 40;
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


is($ngr->rows, 9);

my $range = $ngr->timespan(start  => $ct,
			   end    => $it - 60,
			  );

isa_ok($range, "NGR::Range");
is($range->rows, 9);

for(1..9) {
  my $entry = $range->entry(column => 0, idx => $_-1);

  is_deeply($entry, {
		     stddev => 0,
		     min    => $_,
		     value  => $_,
		     max    => $_,
		     avg    => $_,
		     idx    => $_-1,
		     time   => 0,
		     rows_averaged => 0,
		    }, "entry, this is not aggregated");
}


my $agg = $range->aggregate(interval => 120);

isa_ok($range, "NGR::Range");
is($agg->rows, 5);

{
  my $entry = $agg->entry(column => 0, idx => 0);
  is($entry->{stddev}, 0.5, "stddev");
  is($entry->{min}, 1, "lowest we have seen");
  is($entry->{max}, 2, "highest we have seen");
  is($entry->{avg}, 1.5 , "between 1 and 2");
  is($entry->{rows_averaged}, 2, "this bucket should have two entries!");
}

{
  my $entry = $agg->entry(column => 0, idx => 1);
  is($entry->{stddev}, 0.5, "stddev");
  is($entry->{min}, 3, "lowest we have seen");
  is($entry->{max}, 4, "highest we have seen");
  is($entry->{avg}, 3.5 , "between 3 and 4");
  is($entry->{rows_averaged}, 2, "this bucket should have two entries!");
}

{
  my $entry = $agg->entry(column => 0, idx => 2);
  is($entry->{stddev}, 0.5, "stddev");
  is($entry->{min}, 5, "lowest we have seen");
  is($entry->{max}, 6, "highest we have seen");
  is($entry->{avg}, 5.5 , "between 5 and 6");
  is($entry->{rows_averaged}, 2, "this bucket should have two entries!");
}

{
  my $entry = $agg->entry(column => 0, idx => 3);
  is($entry->{stddev}, 0.5, "stddev");
  is($entry->{min}, 7, "lowest we have seen");
  is($entry->{max}, 8, "highest we have seen");
  is($entry->{avg}, 7.5 , "between 7 and 8");
  is($entry->{rows_averaged}, 2, "this bucket should have two entries!");
}

{
  my $entry = $agg->entry(column => 0, idx => 4);
  is($entry->{stddev}, 0, "no stddev");
  is($entry->{min}, 9, "lowest we have seen");
  is($entry->{max}, 9, "highest we have seen");
  is($entry->{avg}, 9 , "only one in this bucket 9");
  is($entry->{rows_averaged}, 1, "this bucket should have two entries!");
}

END { unlink("aggtest.ngrd") }
