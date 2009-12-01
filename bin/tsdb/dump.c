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
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>

extern char *optarg;

int dump_usage () {
  WARN("Usage: tsdb dump [options]\n");
  WARN(" -f filename  db to get info about\n");
  WARN(" -c column    which column to dump (defaults to dumping all columns)\n");
  WARN(" -d           dump as difference rather than absolute value\n");
  WARN(" -h this help\n");
  return 1;
}

int dump_main(int argc, char * const *argv) {
  int o, column, column_set, difference;
  char *filename = 0;
  difference = column = column_set = 0;

  while ((o = getopt(argc, argv,
		     "c:f:hd")) != -1) {

    switch(o) {
    case 'd':
      difference = 1;
      break;
    case 'f':
      filename = malloc(strlen(optarg)+1);
      memcpy(filename, optarg, strlen(optarg)+1);
      break;
    case 'c':
      column = atoi(optarg);
      column_set = 1;
      break;
    }
  }
  
  if (!filename) {
    return dump_usage();
  }

  struct TSDB_metric_t *metric = TSDB_open(filename);
  struct TSDB_range_t *range = TSDB_range(metric, 0, TSDB_last_row_idx(metric, 0));
  int rows = range->rows;
  int columns = range->columns;
  int i = 0;
  u_int64_t wrap64 = -1;
  u_int32_t wrap32 = -1;

  u_int64_t *last_value = calloc(sizeof(u_int64_t), columns);
  u_int64_t *last_diff  = calloc(sizeof(u_int64_t), columns);
  if(column_set) {
    while(rows--) {
      printf("%llu\n", range->row[(i++ * columns) + column]);
    }
  } else {
    int col = 0;
    while(rows--) {
      while(col < columns) {
	u_int64_t value = range->row[(i * columns) + col];
 
	if (difference) {
	  if (value == 0) {
	    printf("%llu\t", last_diff[col++]);
	    continue;
	  }
	  
	  if (last_value[col] > value) {
	    if (last_value[col] < wrap32) {
	      last_diff[col] = wrap32 - last_value[col] + value;
	    } else {
	      last_diff[col] = wrap64 - last_value[col] + value;
	    }
	  } else if(last_value[col] == 0) {
	    last_value[col++] = value;
	    continue;
	  } else {
	    last_diff[col] = value - last_value[col];
	  }
	  printf("%llu\t", last_diff[col]);

	} else
	  printf("%llu\t", value);

	if (value)
	  last_value[col] = value;
	col++;
      }
      i++;
      col = 0;
      printf("\n");
    }
  }
  return 0;
}
