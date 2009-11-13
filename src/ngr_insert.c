
#include "NGR.h"
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>

extern char *optarg;

int main(int argc, char * const *argv) {
  int o, value;
  time_t insert_time;
  char *filename;

  insert_time = value = 0;

  while ((o = getopt(argc, argv,
		     "f:t:v:")) != -1) {

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
  assert(filename);

  struct NGR_metric_t *metric = NGR_open(filename);

  NGR_insert(metric, 0, insert_time, value);

  return 0;
}
