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

int agg_usage () {
  printf("Usage:\n");
  printf(" -f filename  db to get info about\n");
  printf(" -s start aggregating from this timestamp (unix timestamp)\n");
  printf(" -e aggregate until this timestamp (unix timestamp\n");
  printf(" -i interval to aggregate down to (seconds)\n");
  printf(" -h this help\n\n\n");
  printf("Aggregate a 24 hour period into 12 buckets each with the aggregate of an hour\n");
  printf("\tngr agg -f data.ngr -s 1258096151 -e 1258182558 -i 3600\n\n");
  return 1;
}

int agg_main(int argc, char * const *argv) {
  int o, start, end, interval;
  char *filename = 0;
  
  start = end = interval = 0;

  while ((o = getopt(argc, argv,
		     "f:s:e:i:h")) != -1) {

    switch(o) {
    case 'f':
      filename = malloc(strlen(optarg)+1);
      memcpy(filename, optarg, strlen(optarg)+1);
      break;
    case 's':
      start = atoi(optarg);
      break;      
    case 'e':
      end = atoi(optarg);
      break;      
    case 'i':
      interval = atoi(optarg);
      break;      
    }
  }
  
  if(!filename || !start || !end || !interval)
    return agg_usage();


  struct NGR_metric_t *metric    = NGR_open(filename);
  struct NGR_range_t  *range     = NGR_timespan(metric, 0, start, end);
  struct NGR_range_t  *aggregate = NGR_aggregate(range, interval, NGR_GAUGE);

  int items = aggregate->items;
  int i = 0;
    while(items--) {
      printf("AVG: %d      MAX: %d      MIN: %d      STDDEV: %d\n", aggregate->agg[i].avg, aggregate->agg[i].max, aggregate->agg[i].min, aggregate->agg[i].stddev);
      i++;
    }
  return 0;
}
