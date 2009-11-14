
INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS = ngr

ngr_SOURCES  = ngr.c ngr_info.c ngr_insert.c ngr_create.c ngr_dump.c ngr_agg.c

lib_LTLIBRARIES = libNGR_store.la

libNGR_store_la_LDFLAGS = -version-info 0:0:0

libNGR_store_la_SOURCES = NGR_store.c

LDADD = libNGR_store.la
