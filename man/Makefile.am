if HAVE_POD2MAN
PODSRC = tsdb.1.pod \
           TSDB_create_opts.3.pod TSDB_free_opts.3.pod TSDB_create.3.pod TSDB_open.3.pod \
           TSDB_last_row_idx.3.pod TSDB_cell.3.pod \
           TSDB_range.3.pod TSDB_timespan.3.pod  TSDB_aggregate.3.pod \
           TSDB_insert.3.pod
else
PODSRC =
endif

NUMBEREDMANS=$(PODSRC:.pod=)
MANPAGES=$(PODSRC:.pod=.man)
SUFFIXES : .pod .man .1 .3
CLEANFILES=*.[1-9] *.man

.pod.man :
	-rm -f $@
	pod2man --center="TimeSpliceDB Documentation" --lax --release='$(VERSION)' $< > $@
	cp $@ `echo "$@" | sed -e 's/\.man//'`

%.1 :
	touch $@.pod

%.3 :
	touch $@.pod


all : $(MANPAGES)

if HAVE_GNUMAKE
dist_man_MANS=$(NUMBEREDMANS)
else
# This is slightly weird and ends up with
# man pages called tsdb.1.1 and TSDB_create_opts.3.3
# but they still work so ...
man1_MANS = $(MANPAGES:M*.1.man)
man3_MANS = $(MANPAGES:M*.3.man)
endif

EXTRA_DIST = $(NUMBEREDMANS)
