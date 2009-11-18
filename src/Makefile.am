SUBDIRS  = .
INCLUDES = -I$(top_srcdir)/include 


lib_LTLIBRARIES         = libTSDB_store.la

# TODO this is horrible - in later automakes you
# can just use commands/<foo>.c in ngr_SOURCES
# but in earlier version we have to do this
# except it leaks symbols into libNGR_store
# but LDADD = commands/libNGR_commands.la doesn't work 
# for some reason
libTSDB_store_la_LDFLAGS = -version-info 0:0:0
libTSDB_store_la_SOURCES = TSDB_store.c

LDADD                    = libTSDB_store.la

