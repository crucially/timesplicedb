
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
  int o, start, end, interval;
  char *filename;
  
  start = end = interval = 0;

  while ((o = getopt(argc, argv,
		     "f:")) != -1) {

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
  
  assert(filename);
  assert(start);
  assert(end);
  assert(interval);


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
