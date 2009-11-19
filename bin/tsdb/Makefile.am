
INCLUDES = -I$(top_srcdir)/include 

bin_PROGRAMS = tsdb
tsdb_SOURCES = tsdb.c agg.c bench.c  create.c dump.c info.c insert.c
tsdb_LDFLAGS = -export-dynamic -lm -lc @LIBDL@
tsdb_LDADD = $(top_builddir)/src/libTSDB_store.la
