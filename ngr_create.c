
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
  int o, resolution;
  time_t created_time;
  char *collection, *metric_s;

  created_time = resolution = 0;

  while ((o = getopt(argc, argv,
		     "c:m:r:s:")) != -1) {

    switch(o) {
    case 'c':
      collection = malloc(strlen(optarg)+1);
      memcpy(collection, optarg, strlen(optarg)+1);
      break;
    case 'm':
      metric_s = malloc(strlen(optarg)+1);
      memcpy(metric_s, optarg, strlen(optarg)+1);
      break;
    case 'r':
      resolution = atoi(optarg);
      break;
    case 's':
      created_time = atoi(optarg);
    }

  }
  
  assert(collection);
  assert(metric_s);

  struct NGR_metric_t *metric = NGR_create(collection, metric_s, created_time, resolution);

  time_t last_entry = (metric->created + (NGR_last_entry_idx(metric) * 60));

  printf("Starting time: %s", ctime(&(metric->created)));
  printf("Last entry:    %s", ctime(&last_entry)); 
  printf("Items:         %d\n", NGR_last_entry_idx(metric));
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
