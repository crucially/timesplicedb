

use Test::More tests => 18;
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

for(1..9) {
  my $entry = $range->entry(column => 0, idx => $_-1);

  is_deeply($entry, {
		     stddev => 0,
		     min    => $_,
		     value  => $_,
		     max    => $_,
		     idx    => $_-1,
		     time   => 0,
		     items_averaged => 0,
		    }, "entry, this is not aggregated");
}


my $agg = $range->aggregate(interval => 120);

isa_ok($range, "NGR::Range");
is($agg->items, 5);

{
  my $entry = $agg->entry(column => 0, idx => 0);
  is($entry->{min}, 1, "lowest we have seen");
  {
    local $TODO = "broken in libngr";
    is($entry->{max}, 2, "highest we have seen");
    is($entry->{avg}, 1.5 , "between 1 and 2");
    is($entry->{items_averaged}, 2, "this bucket should have two entries!");
  }
  use Data::Dumper;
#  print Dumper ($entry);
}



END { unlink("aggtest.ngrd") }
