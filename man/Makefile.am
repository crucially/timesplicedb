if HAVE_POD2MAN
dist_man_MANS = tsdb.1 \
		   TSDB_create_opts.3 TSDB_free_opts.3 TSDB_create.3 TSDB_open.3 \
	       TSDB_last_row_idx.3 TSDB_cell.3 \
		   TSDB_range.3 TSDB_timespan.3  TSDB_aggregate.3 \
		   TSDB_insert.3 
else
dist_man_MANS = 
endif

CLEANFILES=*.[1-9]

%.1 :
	-rm -f $@
	pod2man --center="TimeSpliceDB Documentation" --lax --release='$(VERSION)' $(@).pod > $@

%.3 :
	-rm -f $@
	pod2man --center="TimeSpliceDB Documentation" --lax --release='$(VERSION)' $(@).pod > $@


all: $(dist_man_MANS)
	
EXTRA_DIST = $(dist_man_MANS)
