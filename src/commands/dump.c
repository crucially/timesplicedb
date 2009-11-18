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
  WARN(" -f filename  db to get info about\n");
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
	WARN("Usage: ngr dump [options]\n");
    return dump_usage();
  }

  struct TSDB_metric_t *metric = TSDB_open(filename);
  struct TSDB_range_t *range = TSDB_range(metric, 0, TSDB_last_row_idx(metric, 0));
  int rows = range->rows;
  int columns = range->columns;
  int i = 0;
  int *last_value = calloc(sizeof(int), columns);
  int *last_diff  = calloc(sizeof(int), columns);
  if(column_set) {
    while(rows--) {
      printf("%u\n", range->row[(i++ * columns) + column]);
    }
  } else {
    int col = 0;
    while(rows--) {
      while(col < columns) {
	unsigned int value = range->row[(i * columns) + col];
	
	if (difference) {
	  if (value == 0) {
	    printf("%u\t", last_diff[col++]);
	    continue;
	  }
	  if (last_value[col] > value) {
	    /* wrap around */
	    last_diff[col] = 4294967295 - last_value[col] + value;
	  } else {
	    last_diff[col] = value - last_value[col];
	  }
	  printf("%u\t", last_diff[col]);

	} else
	  printf("%u\t", value);
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
