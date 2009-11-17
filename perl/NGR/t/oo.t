#!/usr/bin/perl



use Test::More tests => 17;
BEGIN { use_ok('NGR') };


my $ct = time()-86400;

my $ngr = NGR->new('ootest.ngrd',
    				clobber     => 1,
    				resolution  => 120,
    				columns     => [ 'test' ],
    				create_time => $ct);


isa_ok($ngr, 'NGR');

is($ngr->created, $ct);
is($ngr->resolution, 120);
is($ngr->version, 1);
is($ngr->columns, 1);
is($ngr->rows, 0, "no entries have been added");
is($ngr->last_row_idx, -1, "there is no last idx");

is($ngr->cell(0, 0), 0)


$ngr->insert(0 => 1, $ct+51);                           # insert with column offset instead of name

is($ngr->rows, 1, "there is an actual item in the first slot");
is($ngr->last_row_idx, 0, "the index of said slot is still 0"); 


$ngr->insert('test' => $value, $ct + $ngr->resolution); # insert with column name instead of offset

is($ngr->cell(0, 'test'), 1, "correct column lookup with name");
is($ngr->cell(1, 0),      2, "correct column lookup with offset");

is($ngr->last_row_idx, 1, ""); 
is($ngr->rows, 2);

my $it = $ct + 86400;
$ngr->insert('test' => 10, $it - 1);

my $info = $ngr->info();

is_deeply($info, {
    rows         => 720,
    created      => $ct,
    columns      => 1,
    resolution   => 120,
    version      => 1,
    columns      => 1,
    last_updated => $it - 120,
	});

is($ngr->last_updated, $it - 120, "Last modified is the value of the row");
END {
    unlink("ootest.ngrd");
};
