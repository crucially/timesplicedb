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

int usage () {
  printf("Usage:\n");
  printf(" -f filename  db to get info about\n");
  printf(" -r resolution for each entry in the time series (seconds)\n");
  printf(" -c the time of the first entry in the time series (unix timestamp)\n");
  printf(" -h this help\n\n\n");
  printf("Create a database starting at a given time with a storage interval of every 10 minutes\n");
  printf("\tngr_create -f data.ngr -c 1258096151 -r 600\n\n");
  return 1;
}

int main(int argc, char * const *argv) {
  int o, resolution;
  time_t created_time;
  char *filename = 0;

  created_time = resolution = 0;

  while ((o = getopt(argc, argv,
		     "f:r:c:h")) != -1) {

    switch(o) {
    case 'f':
      filename = malloc(strlen(optarg)+1);
      memcpy(filename, optarg, strlen(optarg)+1);
      break;
    case 'r':
      resolution = atoi(optarg);
      break;
    case 'c':
      created_time = atoi(optarg);
      break;
    case 'h':
      return usage();
      break;
    }

  }
  
  if(!filename)
    return usage();

  struct NGR_metric_t *metric = NGR_create(filename, created_time, resolution, 0);

  time_t last_entry = (metric->created + (NGR_last_entry_idx(metric, 0) * 60));

  printf("Starting time: %s", ctime(&(metric->created)));
  printf("Last entry:    %s", ctime(&last_entry)); 
  printf("Items:         %d\n", NGR_last_entry_idx(metric, 0));
  printf("Resolution:    %d seconds\n", metric->resolution);
  printf("Verison:       %d\n", metric->version);
  if (metric->width == 8) {
    printf("Format:        64bit\n");
  } else if (metric->width == 4) {
    printf("Format:        32bit\n");
  } else {
    printf("Format:        unknown!\n");
  }

  return 0;
}
