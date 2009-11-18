SUBDIRS  = commands .
INCLUDES = -I$(top_srcdir)/include


bin_PROGRAMS = tsdb
tsdb_SOURCES  = tsdb.c 
tsdb_LDFLAGS  = -lm -ldl

lib_LTLIBRARIES         = libTSDB_store.la

libTSDB_store_la_LDFLAGS = -version-info 0:0:0
libTSDB_store_la_SOURCES = TSDB_store.c
libTSDB_store_la_LIBADD  = commands/libTSDB_commands.la

LDADD                   = libTSDB_store.la


