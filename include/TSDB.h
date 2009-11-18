/*
 * Copyright (c) 2009 Artur Bergman <sky@crucially.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>


#include <sys/uio.h>


#define TSDB_GAUGE   0
#define TSDB_COUNTER 1

#define TSDB_NULL (0)
#define WARN(args...) fprintf(stderr, ## args);
#define strneq(s1, s2, n) (strncmp((s1), (s2), n) == 0)


struct TSDB_metric_t {
  unsigned   magic;
#define TSDB_metric_magic 0x35fac554

  u_int32_t  width;      /* are we in 32bit mode or 64bit  (4 bytes versus 8 bytes) */
  int        fd;         /* the underlying file */
  int        base;       /* how many bytes the header consumes */
  u_int64_t  created;    /* timestamp the first row in the series is */
  int        resolution; /* distance in seconds between each row in the series */
  int        version;    /* storage verson */
  char       *name;
  int        flags;
  int        columns;    /* how many columns this store has */
  int        *col_flags; /* first is flag for db, rest is per column */
  char       **col_names;/* first entry is the database name, rest is the column names */
  int        tsdberrno;  /* set to non 0 if the latest function recorded an error */
  int        syserrno;   /* underlying errno if result of a syscall */
  int        ranges;     /* refcount off ranges */
};

struct TSDB_range_t {
  unsigned   magic;
#define TSDB_range_magic 0x6428b5c9

  u_int64_t *row;              /* ptr into the first row in the range */
  void *area;                  /* if a range and a not an aggregate this points to the mmap of the file */ 
  int rows;                    /* how many rows exist in the range */
  size_t len;
  int mmap;                    /* if this is an mmaped range or not */
  int resolution;              /* resolution requested for this range  */
  int columns;                 /* how many columns are present in this range */
  struct TSDB_agg_row_t *agg;  /* If this is an aggregate range, this contains a pointer to extra data 
				  calculated during aggregaton
				  these are values we can get doing a single pass only
				  the reason this datacenter is dual purpose
				  is so you can call aggregate on already aggregated ranges
			       */
  struct TSDB_metric_t *metric; /* where did we come from ? */
};

struct TSDB_create_opts_t {
  unsigned        magic;
#define TSDB_create_opts_magic 0x7428b5c9
  char           *filename;
  u_int64_t      created_time;
  unsigned int   resolution;
  unsigned int   columns;
  char           *name;
  unsigned int   flags;
  char           **col_names;
  int            *col_flags;
};

struct TSDB_agg_row_t {
  double avg;        /* average value in interval */
  u_int64_t max;     /* max value seen in interval */
  u_int64_t min;     /* minimum value seen -- flag determines if 0 is considered minium or undefined value */
  double stddev;     /* calculated stddev -- no idea if this is correct -- probably isn't */
  int rows_averaged; /* how many rows went into this */
};

struct TSDB_create_opts_t * TSDB_create_opts(unsigned int columns);

void TSDB_free_opts(struct TSDB_create_opts_t *opts);

struct TSDB_metric_t * TSDB_create(struct TSDB_create_opts_t *opts);

struct TSDB_metric_t * TSDB_open(const char *filename);


int TSDB_last_row_idx (struct TSDB_metric_t *obj, int column);

u_int64_t TSDB_cell (struct TSDB_metric_t *obj, int row, int column);

struct TSDB_range_t * TSDB_range (struct TSDB_metric_t *obj, int start, int end);

void TSDB_range_free (struct TSDB_range_t *range);

struct TSDB_range_t * TSDB_timespan (struct TSDB_metric_t *obj, time_t start, time_t end);

struct TSDB_range_t * TSDB_aggregate (struct TSDB_range_t *range, int interval, int data_type);

int TSDB_insert (struct TSDB_metric_t *obj, int column, time_t timestmp, u_int64_t value);
