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

#include "TSDB.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#include <string.h>





/* file format is pretty simple
   4 bytes for the width of the fields (32/64)
   4 bytes for the version number of format
   4 bytes for the resolution in seconds (resolution is not changeable once a file is created
   4 bytes for the number of columns
   $width create_timestamp unix_time when the series start
   list of items, indexed of the time-create_time/interval
*/

struct TSDB_create_opts_t * TSDB_create_opts(unsigned int columns) {
  struct TSDB_create_opts_t *opts = calloc(1, sizeof(struct TSDB_create_opts_t));
  opts->magic     = TSDB_create_opts_magic;
  opts->columns   = columns;
  opts->col_names = calloc(columns, sizeof(char *));
  opts->col_flags = calloc(columns, sizeof(int));
  return opts;
}

void TSDB_free_opts(struct TSDB_create_opts_t *opts) {
  free(opts->col_flags);
  free(opts->col_names);
  free(opts);
  return;
}

struct TSDB_metric_t * TSDB_create(struct TSDB_create_opts_t *opts) {
  int   fd, write_len,len;
  int   size = sizeof(unsigned long long);
  int   version = 1;

  assert(opts->magic == TSDB_create_opts_magic);

  assert(opts->columns > 0);

  if(!opts->created_time)
    opts->created_time = time(NULL);

  if(!opts->resolution)
    opts->resolution = 60;
  
  fd = open(opts->filename, O_CREAT | O_RDWR | O_EXCL, 0755);

  if(fd == -1)
    printf("Cannot open %s (%s)\n", opts->filename, strerror(errno));
  assert(fd != -1);

  
  write_len = write(fd, &size, 4);
  assert(write_len == 4);
  write_len = write(fd, &version, 4);
  assert(write_len == 4);
  write_len = write(fd, &opts->resolution, 4);
  assert(write_len == 4);
  write_len = write(fd, &opts->columns, 4);
  assert(write_len == 4);
  write_len = write(fd, &opts->created_time, size);
  assert(write_len == size);

  len = strlen(opts->name) + 1;
  write_len = write(fd, &opts->flags, 4);
  assert(write_len == 4);
  write_len = write(fd, &len, 4);
  assert(write_len == 4);
  write_len = write(fd, opts->name, strlen(opts->name)+1);
  assert(write_len == strlen(opts->name)+1);


  int i;
  for(i = 0; i < opts->columns; i++) {
    len = strlen(opts->col_names[i]) + 1;
    write_len = write(fd, &opts->col_flags[i], 4);
    assert(write_len == 4);
    write_len = write(fd, &len, 4);
    assert(write_len == 4);
    write_len = write(fd, opts->col_names[i], len);
    assert(write_len == len);
  }


  close(fd);

  return TSDB_open(opts->filename);
}

struct TSDB_metric_t * TSDB_open(const char *filename) {
  int len;
  size_t read_len;
  struct TSDB_metric_t *obj;

  obj = malloc(sizeof(struct TSDB_metric_t));
  obj->magic = TSDB_metric_magic;
  obj->fd = open(filename, O_RDWR);

  if(obj->fd == -1)
    printf("Cannot open %s (%s)\n", filename, strerror(errno));
  assert(obj->fd != -1);

  obj->base = 4;
  read_len = read(obj->fd, &obj->width, 4);
  assert(read_len == 4);

  obj->base += 4;
  read_len = read(obj->fd, &obj->version, 4);
  assert(read_len == 4);

  obj->base += 4;
  read_len = read(obj->fd, &obj->resolution, 4);
  assert(read_len == 4);

  obj->base += 4;
  read_len = read(obj->fd, &obj->columns, 4);
  assert(read_len == 4);
  

  obj->base += obj->width;
  read_len = read(obj->fd, &obj->created, obj->width);
  assert(read_len == obj->width);

  obj->ranges = 0;

  read_len = read(obj->fd, &obj->flags, 4);
  assert(read_len == 4);
  obj->base += 4;
  read_len = read(obj->fd, &len, 4);
  assert(read_len == 4);
  obj->base += 4;
  obj->name = malloc(len);
  read_len = read(obj->fd, obj->name, len);
  assert(read_len == len);
  obj->base += len;

  obj->col_names = malloc(sizeof(char *) * (obj->columns));
  obj->col_flags = malloc(sizeof(int) * (obj->columns));
  int i;
  for(i = 0; i < obj->columns; i++) {
    read_len = read(obj->fd, &obj->col_flags[i], 4);
    assert(read_len == 4);
    obj->base += 4;
    read_len = read(obj->fd, &len, 4);
    assert(read_len == 4);
    obj->base += 4;
    obj->col_names[i] = malloc(len);
    read_len = read(obj->fd, obj->col_names[i], len);
    assert(read_len == len);
    obj->base += len;
  }


  assert(obj->width == 4 || obj->width == 8);

  return obj;
}

int TSDB_close(struct TSDB_metric_t *obj) {
  assert(obj->magic == TSDB_metric_magic);
  assert(obj->ranges == 0);
  close(obj->fd);
  free(obj);
}

/* 
   column     which column to write in
   timestamp  which timestamp this is for, defaults to now if not set
   value      value to store
*/

int TSDB_insert (struct TSDB_metric_t *obj, int column, time_t timestamp, u_int64_t value) {
  int  row, offset, write_len;

  assert(obj->magic == TSDB_metric_magic);

  if (!timestamp)
    timestamp = time(NULL);

  assert (column <= obj->columns - 1);
  assert( timestamp >= obj->created );
  assert (timestamp <= time(NULL) );

  /* We have to transform the timestamp to a row index and then into a byte offset
     This is rather trivial because we know the value that it was created by, and we can 
     calculate the difference, and then turn that into a byte offset */

  row  = ((timestamp - obj->created) / obj->resolution);
  offset = (obj->base + ( row * ( obj->width * obj->columns )) + ( column * obj->width ));
  lseek(obj->fd, offset, SEEK_SET);
  write_len = write(obj->fd, &value, obj->width);
  assert(write_len == obj->width);
  return row;
}


/*
  column    which column to retrieve
  start     which row to start with, indexed from 0
  end       what row to stop at
*/

struct TSDB_range_t * TSDB_range (struct TSDB_metric_t *obj, int start, int end) {
  struct TSDB_range_t *range;
  struct stat *file;

  assert(obj->magic == TSDB_metric_magic);

  range = malloc(sizeof(struct TSDB_range_t));
  file = malloc(sizeof(struct stat));
  fstat(obj->fd, file);
  range->magic = TSDB_range_magic;

  /* if the range goes byond the file, map that as well */
  if ((end * obj->width + obj->base) > file->st_size)
    range->len = (end * obj->width + obj->base);
  else
    range->len = file->st_size;

  /* Ideally we would only map the region we need
     byt I haven't gotten around to write the correct
     page alignment code for that.
     We would need to align to the page, and then recalculate
     the offset in the first page.
  */

  free(file);
  range->area = mmap(0, range->len, PROT_READ, MAP_SHARED| MAP_FILE, obj->fd, 0);
  assert(range->area != (void*)-1);
  range->row = (range->area + obj->base + (obj->width * start));
  range->rows = end - start + 1;
  range->mmap = 1;
  range->agg = 0; 
  range->resolution = obj->resolution;
  range->columns = obj->columns;
  range->metric = obj;
  range->metric->ranges++;
  return range;
}


/*
  frees a given range
  if it is mmaped then free the are
  aggregates are not mmaped, pure ranges are
*/

void TSDB_range_free (struct TSDB_range_t * range) {
  assert(range->magic == TSDB_range_magic);
  range->metric->ranges--;

  if (range->mmap == 1)
    munmap(range->area, range->len);
  else
    free(range->area);

  if(range->agg)
    free(range->agg);

  free(range);
}


/* 
   column    which column
   start     what time this starts from
   stop      and last time we want
*/
struct TSDB_range_t * TSDB_timespan (struct TSDB_metric_t *obj, time_t start, time_t end) {
  int start_offset, end_offset;

  assert(obj->magic == TSDB_metric_magic);

  /* this just converts the timestamp into an offset that the range function takes */
  start_offset = ((start - obj->created) / obj->resolution);
  end_offset = ((end - obj->created) / obj->resolution);
  return TSDB_range(obj, start_offset, end_offset);
}


int TSDB_last_row_idx (struct TSDB_metric_t *obj, int column) {
  int offset;
  assert(obj->magic == TSDB_metric_magic);
  assert (column <= obj->columns - 1);
  offset = lseek(obj->fd, 0 - (obj->width * obj->columns), SEEK_END);
  if(offset < obj->base)
    return -1;
  return (((int)offset - obj->base) / (obj->width * obj->columns)); /** is this really supposed to be please +1 **/
}


u_int64_t TSDB_cell (struct TSDB_metric_t *obj, int row, int column) {
  char *buf;
  u_int64_t rv;
  int read_len, offset;
  assert(obj->magic == TSDB_metric_magic);
  assert (column <= obj->columns - 1);
  offset = (obj->base + (row * ( obj->width * obj->columns)) + (column * obj->width));
  buf = malloc(obj->width);
  assert(sizeof(rv) == obj->width);
  lseek(obj->fd, offset, SEEK_SET);

  read_len = read(obj->fd, buf, obj->width);
  if (read_len == 0) {
    /* we are outside the length of the file
       should really check if this is the case XXX */
    rv = 0;
  } else {
    memcpy(&rv, buf, obj->width);
  }
  free(buf);
  return rv;
}

struct TSDB_agg_counters_t {
  unsigned int cells_counted;  /* how many cells we have counted */
  u_int64_t sum;            /* sum */
  u_int64_t min;            /* minum value seen */
  u_int64_t max;            /* maximum value seen */
  double sum_sqr;
  u_int64_t last_value;
};


struct TSDB_range_t * TSDB_aggregate (struct TSDB_range_t *range, int interval, int data_type) {
  struct TSDB_range_t *aggregate;
  struct TSDB_agg_counters_t *counters;
  assert(range->magic == TSDB_range_magic);
  int src_rows, src_cells, curr_cell, trg_cell;
  src_rows = src_cells = curr_cell = trg_cell = 0;

  counters = calloc(range->columns, sizeof(struct TSDB_agg_counters_t));

  src_rows = range->rows;
  src_cells = src_rows * range->columns;

  int i = 0;
  while(i < range->columns) {
    counters[i].last_value = counters[i].cells_counted = counters[i].sum = counters[i].max = counters[i].sum_sqr = 0;
    counters[i].min = 4294967295; /** broken on 64bit, i know, and I haven't how to deal with signed or unsigned yet probably counters
			are unsigned and gauge signed?**/

    i++;
  }

  /* figure out how many buckets we need, switch to floating point and then round up */
  int buckets = rint(ceil(((double)range->rows / ((double)interval / (double)range->resolution))));

  aggregate = malloc(sizeof(struct TSDB_range_t));
  aggregate->area = malloc(sizeof(u_int64_t) * buckets * range->columns);
  aggregate->agg = malloc(sizeof(struct TSDB_agg_row_t) * buckets * range->columns);
  aggregate->mmap = 0;
  aggregate->rows = buckets;
  aggregate->row = aggregate->area;
  aggregate->columns = range->columns;
  aggregate->metric = range->metric;
  aggregate->metric->ranges++;
  int rows_per_bucket = ((interval/range->resolution));
  int cells_seen = 0;

  while(src_cells--) {
    u_int64_t value;
    struct TSDB_agg_counters_t *counter = (counters + (curr_cell % range->columns));

    if (data_type == TSDB_GAUGE)
      value = range->row[curr_cell];
    else  {
      int new_value = range->row[curr_cell];
      int old_value = range->row[curr_cell - range->columns];
      if (range->row[curr_cell] == 0 || range->row[curr_cell - range->columns] == 0) {
	value = counters->last_value;
      } else {
	if (old_value > new_value)
	  value = 4294967295 - old_value + new_value;
	else 
	  value = new_value - old_value;
      }
    }


    counter->sum += value;
    counter->cells_counted++;

    counter->sum_sqr += (double)value * (double)value;

    if (counter->min > value)
      counter->min = value;
    if (counter->max < value)
      counter->max = value;

    if(cells_seen++ == rows_per_bucket * range->columns - 1) {
      int i = 0;
      while(i < range->columns) {
	struct TSDB_agg_counters_t *column = counters + i;
	double avg = (double)column->sum / (double)column->cells_counted;

	aggregate->agg[trg_cell].rows_averaged = column->cells_counted;
	aggregate->row[trg_cell] = column->sum / column->cells_counted;
	aggregate->agg[trg_cell].max = column->max;
	aggregate->agg[trg_cell].min = column->min;

	if (column->cells_counted == 1)
	  aggregate->agg[trg_cell].stddev = 0;
	else 
	  aggregate->agg[trg_cell].stddev = ((column->sum_sqr - column->sum * avg)/(column->cells_counted - 1));
	
	aggregate->agg[trg_cell++].avg = avg;

	column->sum_sqr = column->cells_counted = column->sum = column->max = 0;
	column->min = 2147483647; /** broken on 64bit, i know, and I haven't how to deal with signed or unsigned yet probably counters
			are unsigned and gauge signed?**/

	i++;
      }
      cells_seen = 0;
    }
    counter->last_value = value;
    curr_cell++;
  }
  
  if (cells_seen) {

    int i = 0;
    while(i < range->columns) {
      struct TSDB_agg_counters_t *column = counters + i;
      double avg = (double)column->sum / (double)column->cells_counted;
      
      aggregate->agg[trg_cell].rows_averaged = column->cells_counted;
      aggregate->row[trg_cell] = column->sum / column->cells_counted;
      aggregate->agg[trg_cell].max = column->max;
      aggregate->agg[trg_cell].min = column->min;
      
      if (column->cells_counted == 1)
	aggregate->agg[trg_cell].stddev = 0;
      else 
	aggregate->agg[trg_cell].stddev = ((column->sum_sqr - column->sum * avg)/(column->cells_counted - 1));
      
      aggregate->agg[trg_cell++].avg = avg;
      
      i++;
    }
    
    /* XXX SSHOULD SET THE RESOLUTION ON AGGREGATE */
  }
  
  return aggregate;
}

