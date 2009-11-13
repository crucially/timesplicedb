
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
  struct NGR_range_t *range = NGR_range(metric, 0, 0, NGR_last_entry_idx(metric, 0));

  int items = range->items;
  int i = 0;
    while(items--) {
      printf("%d\n", range->entry[i++]);
    }
  
  return 0;
}
