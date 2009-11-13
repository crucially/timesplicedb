
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
  printf(" -s start aggregating from this timestamp (unix timestamp)\n");
  printf(" -e aggregate until this timestamp (unix timestamp\n");
  printf(" -i interval to aggregate down to (seconds)\n");
  printf(" -h this help\n\n\n");
  printf("Aggregate a 24 hour period into 12 buckets each with the aggregate of an hour\n");
  printf("\tngr_agg -f data.ngr -s 1258096151 -e 1258182558 -i 3600\n\n");
  return 1;
}

int main(int argc, char * const *argv) {
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
    return usage();


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
