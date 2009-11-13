
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
  int o;
  char *filename;

  while ((o = getopt(argc, argv,
		     "f:")) != -1) {

    switch(o) {
    case 'f':
      filename = malloc(strlen(optarg)+1);
      memcpy(filename, optarg, strlen(optarg)+1);
      break;
    }
  }
  
  assert(filename);

  struct NGR_metric_t *metric = NGR_open(filename);
  time_t last_entry = (metric->created + (NGR_last_entry_idx(metric, 0) * metric->resolution));

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
