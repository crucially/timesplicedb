
#include "NGR.h"
#include <sys/time.h>
#include <stdio.h>;
#include <time.h>;
#include <assert.h>;
#include <string.h>

#include <stdlib.h>;
#include <unistd.h>

extern char *optarg;

int main(int argc, char * const *argv) {
  int o, value;
  time_t insert_time;
  char *collection, *metric_s;

  insert_time = value = 0;

  while ((o = getopt(argc, argv,
		     "c:m:t:v:")) != -1) {

    switch(o) {
    case 'c':
      collection = malloc(strlen(optarg)+1);
      memcpy(collection, optarg, strlen(optarg)+1);
      break;
    case 'm':
      metric_s = malloc(strlen(optarg)+1);
      memcpy(metric_s, optarg, strlen(optarg)+1);
      break;
    case 't':
      insert_time = atoi(optarg);
      break;
    case 'v':
      value = atoi(optarg);
    }

  }
  
  assert(collection);
  assert(metric_s);

  struct NGR_metric_t *metric = NGR_open(collection, metric_s);

  NGR_insert(metric, 0, insert_time, value);

  return 0;
}
