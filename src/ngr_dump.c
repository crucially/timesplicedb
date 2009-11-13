
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
  printf(" -h this help\n");
  return 1;
}

int main(int argc, char * const *argv) {
  int o;
  char *filename = 0;

  while ((o = getopt(argc, argv,
		     "f:h")) != -1) {

    switch(o) {
    case 'f':
      filename = malloc(strlen(optarg)+1);
      memcpy(filename, optarg, strlen(optarg)+1);
      break;
    }
  }
  
  if (!filename)
    return usage();


  struct NGR_metric_t *metric = NGR_open(filename);
  struct NGR_range_t *range = NGR_range(metric, 0, 0, NGR_last_entry_idx(metric, 0));

  int items = range->items;
  int i = 0;
    while(items--) {
      printf("%d\n", range->entry[i++]);
    }
  
  return 0;
}
