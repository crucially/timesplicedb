SUBDIRS  = commands collectd .
INCLUDES = -I$(top_srcdir)/include 


bin_PROGRAMS = ngr
ngr_SOURCES  = ngr.c 
ngr_LDFLAGS  = -lm -ldl

lib_LTLIBRARIES         = libNGR_store.la

libNGR_store_la_LDFLAGS = -version-info 0:0:0
libNGR_store_la_SOURCES = NGR_store.c

# TODO this is horrible - in later automakes you
# can just use commands/<foo>.c in ngr_SOURCES
# but in earlier version we have to do this
# except it leaks symbols into libNGR_store
# but LDADD = commands/libNGR_commands.la doesn't work 
# for some reason
libNGR_store_la_LIBADD  = commands/libNGR_commands.la

LDADD                   = libNGR_store.la

