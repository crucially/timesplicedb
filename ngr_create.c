
#include "NGR.h"
#include <sys/time.h>
#include <stdio.h>;
#include <time.h>;
#include <assert.h>;
#include <string.h>

#include <stdlib.h>;

extern char *optarg;

int main(int argc, char * const *argv) {
  int o, resolution;
  time_t created_time;
  char *collection, *item;

  created_time = resolution = 0;

  while ((o = getopt(argc, argv,
		     "i:c:")) != -1) {

    switch(o) {
    case 'c':
      collection = malloc(strlen(optarg+1));
      memcpy(collection, optarg, strlen(optarg)+1);
      break;
    case 'i':
      item = malloc(strlen(optarg+1));
      memcpy(item, optarg, strlen(optarg)+1);
      break;
    }



  }
  
  assert(collection);
  assert(item);

  printf("%s %s %d %d\n", collection, item, created_time, resolution);

  struct NGR_metric_t *metric = NGR_create(collection, item, created_time, resolution);

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


}
