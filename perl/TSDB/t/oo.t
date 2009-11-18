#!/usr/bin/perl



use Test::More tests => 17;
BEGIN { use_ok('TSDB') };


my $ct = time()-86400;

my $tsdb = TSDB->new('ootest.tsdb',
    				clobber     => 1,
    				resolution  => 120,
    				columns     => [ 'test' ],
    				create_time => $ct);


isa_ok($tsdb, 'TSDB');

is($tsdb->created, $ct);
is($tsdb->resolution, 120);
is($tsdb->version, 1);
is($tsdb->columns, 1);
is($tsdb->rows, 0, "no entries have been added");
is($tsdb->last_row_idx, -1, "there is no last idx");

is($tsdb->cell(0, 0), 0);


$tsdb->insert(0 => 1, $ct+51);                           # insert with column offset instead of name

is($tsdb->rows, 1, "there is an actual item in the first slot");
is($tsdb->last_row_idx, 0, "the index of said slot is still 0"); 


$tsdb->insert('test' => $value, $ct + $tsdb->resolution); # insert with column name instead of offset

is($tsdb->cell(0, 'test'), 1, "correct column lookup with name");
is($tsdb->cell(1, 0),      2, "correct column lookup with offset");

is($tsdb->last_row_idx, 1, ""); 
is($tsdb->rows, 2);

my $it = $ct + 86400;
$tsdb->insert('test' => 10, $it - 1);

my $info = $tsdb->info();

is_deeply($info, {
    rows         => 720,
    created      => $ct,
    columns      => 1,
    resolution   => 120,
    version      => 1,
    columns      => 1,
    last_updated => $it - 120,
	});

is($tsdb->last_updated, $it - 120, "Last modified is the value of the row");
END {
    unlink("ootest.tsdb");
};
