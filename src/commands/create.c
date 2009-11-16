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

#include "NGR.h"
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>


extern char *optarg;

int create_usage () {
  WARN(" -f filename  db to get info about\n");
  WARN(" -r resolution for each row in the time series (seconds)\n");
  WARN(" -b the time of the first row in the time series (unix timestamp)\n");
  WARN(" -c the number of columns per row");
  WARN(" -h this help\n\n");
  WARN("Create a database starting at a given time with a storage interval of every 10 minutes\n");
  WARN("\tngr create -f data.ngr -c 1258096151 -r 600\n");
  return 1;
}

int create_main(int argc, char * const *argv) {
  int o, resolution, columns;
  time_t beginning_time;
  char *filename = 0;

  columns = beginning_time = resolution = 0;

  while ((o = getopt(argc, argv,
		     "f:r:b:h:c:")) != -1) {

    switch(o) {
    case 'c':
      columns = atoi(optarg);
      break;
    case 'f':
      filename = optarg;
      break;
    case 'r':
      resolution = atoi(optarg);
      break;
    case 'b':
      beginning_time = atoi(optarg);
      break;
    case 'h':
 	  WARN("Usage: ngr create [options]\n");
      return create_usage();
      break;
    }

  }
  
  if(!filename) {
    WARN("Usage: ngr create [options]\n");
    return create_usage();
  }

  char **foo = malloc(sizeof(char *) * (columns + 1));
  foo[0] = "Database name";
  int j;
  for(j = 1; j <= columns; j++) {
    foo[j] = "Column";
  }


  struct NGR_metric_t *metric = NGR_create(filename, beginning_time, resolution, columns, foo);

  time_t last_row = (metric->created + (NGR_last_row_idx(metric, 0) * 60));

  printf("Name:          %s\n", metric->names[0]);
  printf("Starting time: %s", ctime(&(metric->created)));
  printf("Last row:      %s", ctime(&last_row)); 
  printf("Rows:          %d\n", NGR_last_row_idx(metric, 0) + 1);
  printf("Columns:       %d\n", metric->columns); 
  printf("Resolution:    %d seconds\n", metric->resolution);
  printf("Verison:       %d\n", metric->version);
  if (metric->width == 8) {
    printf("Format:        64bit\n");
  } else if (metric->width == 4) {
    printf("Format:        32bit\n");
  } else {
    printf("Format:        unknown!\n");
  }
  int i;
  for(i = 1; i <= metric->columns; i++) {
    printf("Column %d:      %s\n", i, metric->names[i]);
  }
  return 0;
}
