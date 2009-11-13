
INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS = ngr_info ngr_insert ngr_create ngr_dump ngr_agg

lib_LTLIBRARIES = libNGR_store.la

libNGR_store_la_LDFLAGS = -version-info 0:0:0

libNGR_store_la_SOURCES = NGR_store.c

LDADD = libNGR_store.la
