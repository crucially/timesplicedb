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

int insert_usage () {
  WARN(" -f filename  db to get info about\n");
  WARN(" -t timestamp to insert this value at; defaults to now (unix timestamp)\n");
  WARN(" -v value to insert (integer)\n");
  WARN(" -h this help\n\n");
  WARN("Insert a value at a given time\n");
  WARN("\tngr insert -f data.ngr -t 1258096151 -v 50\n");
  return 1;
}

int insert_main(int argc, char * const *argv) {
  int o, value;
  time_t insert_time;
  char *filename = 0;

  insert_time = value = 0;

  while ((o = getopt(argc, argv,
		     "f:t:v:h")) != -1) {

    switch(o) {
    case 'f':
      filename = malloc(strlen(optarg)+1);
      memcpy(filename, optarg, strlen(optarg)+1);
      break;
    case 't':
      insert_time = atoi(optarg);
      break;
    case 'v':
      value = atoi(optarg);
    }

  }

  if (!filename || !value) {
    WARN("Usage: ngr insert [options]\n");
    return insert_usage();
  }


  struct NGR_metric_t *metric = NGR_open(filename);

  NGR_insert(metric, 0, insert_time, value);

  return 0;
}
