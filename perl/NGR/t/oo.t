#!/usr/bin/perl



use Test::More tests => 17;
BEGIN { use_ok('NGR') };


my $ct = time()-86400;

my $ngr = NGR->new(
    create      => 1,
    filename    => 'ootest.ngrd',
    resolution  => 120,
    columns     => 1,
    create_time => $ct);


isa_ok($ngr, 'NGR');

is($ngr->created, $ct);
is($ngr->resolution, 120);
is($ngr->version, 1);
is($ngr->columns, 1);
is($ngr->items, 0, "no entries have been added");
is($ngr->last_entry_idx, -1, "there is no last idx");

is($ngr->entry(column => 0,
	       idx    => 0), 0);


$ngr->insert(column    => 0,
	     timestamp => $ct+51,
	     value     => 1);

is($ngr->items, 1, "there is an actual item in the first slot");
is($ngr->last_entry_idx, 0, "the index of said slot is still 0"); 


$ngr->insert(column    => 0,
	     timestamp => $ct + $ngr->resolution,
	     value     => 2);

is($ngr->entry(column => 0,
	       idx    => 0), 1);


is($ngr->entry(column => 0,
	       idx    => 1), 2);


is($ngr->last_entry_idx, 1, ""); 
is($ngr->items, 2);

my $it = time();
$ngr->insert(column    => 0,
	     timestamp => $it-1,
	     value     => 10);

my $info = $ngr->info();

is_deeply($info, {
    items        => 720,
    created      => $ct,
    columns      => 1,
    resolution   => 120,
    version      => 1,
    columns      => 1,
    last_updated => $it - 120,
	});

is($ngr->last_updated, $it - 120, "Last modified is the value of the idx");

END {
    unlink("ootest.ngrd");
};
