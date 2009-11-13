
#include "NGR.h"
#include <sys/time.h>
#include <stdio.h>;
#include <time.h>;
#include <assert.h>;
#include <string.h>

#include <stdlib.h>;

extern char *optarg;

int main(int argc, char * const *argv) {
  int o;
  char *collection, *metric_s;

  while ((o = getopt(argc, argv,
		     "c:m:")) != -1) {

    switch(o) {
    case 'c':
      collection = malloc(strlen(optarg)+1);
      memcpy(collection, optarg, strlen(optarg)+1);
      break;
    case 'm':
      metric_s = malloc(strlen(optarg)+1);
      memcpy(metric_s, optarg, strlen(optarg)+1);
      break;
    }
  }
  
  assert(collection);
  assert(metric_s);

  struct NGR_metric_t *metric = NGR_open(collection, metric_s);
  struct NGR_range_t *range = NGR_range(metric, 0, NGR_last_entry_idx(metric));

  int items = range->items;
  int i = 0;
    while(items--) {
      printf("%d\n", range->entry[i++]);
    }

}
