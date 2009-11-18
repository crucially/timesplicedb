SUBDIRS  = commands collectd .
INCLUDES = -I$(top_srcdir)/include 


bin_PROGRAMS = tsdb
tsdb_SOURCES  = tsdb.c 
tsdb_LDFLAGS  = -lm -ldl

lib_LTLIBRARIES         = libTSDB_store.la

<<<<<<< HEAD:src/Makefile.am
# TODO this is horrible - in later automakes you
# can just use commands/<foo>.c in ngr_SOURCES
# but in earlier version we have to do this
# except it leaks symbols into libNGR_store
# but LDADD = commands/libNGR_commands.la doesn't work 
# for some reason
libTSDB_store_la_LDFLAGS = -version-info 0:0:0
libTSDB_store_la_SOURCES = TSDB_store.c
libTSDB_store_la_LIBADD  = commands/libTSDB_commands.la
=======
libTSDB_store_la_LDFLAGS = -version-info 0:0:0
libTSDB_store_la_SOURCES = TSDB_store.c
libTSDB_store_la_LIBADD  = commands/libTSDB_commands.la

LDADD                   = libTSDB_store.la
>>>>>>> 0ac0100d01f05c746e4ac08105cd31cc9effc41e:src/Makefile.am

LDADD                    = libTSDB_store.la

