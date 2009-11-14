SUBDIRS  = commands .
INCLUDES = -I$(top_srcdir)/include


bin_PROGRAMS = ngr
ngr_SOURCES  = ngr.c 
ngr_LDFLAGS  = -lm -ldl

lib_LTLIBRARIES         = libNGR_store.la

libNGR_store_la_LDFLAGS = -version-info 0:0:0
libNGR_store_la_SOURCES = NGR_store.c
libNGR_store_la_LIBADD  = commands/libNGR_commands.la

LDADD                   = libNGR_store.la


