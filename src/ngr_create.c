
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
  char *filename = 0

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
