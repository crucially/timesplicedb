
#include "NGR.h"
#include <sys/time.h>
#include <stdio.h>;
#include <time.h>;
#include <assert.h>;
#include <string.h>

#include <stdlib.h>;
#include <unistd.h>

extern char *optarg;

int usage () {
  printf("Usage:\n");
  printf(" -f filename  db to get info about\n");
  printf(" -t timestamp to insert this value at (unix timestamp)\n");
  printf(" -v value to insert (integer)\n");
  printf(" -h this help\n\n\n");
  printf("Insert a value at a given time\n");
  printf("\tngr_insert -f data.ngr -t 1258096151 -v 50\n\n");
  return 1;
}

int main(int argc, char * const *argv) {
  int o, value;
  time_t insert_time;
  char *filename = 0;

  insert_time = value = 0;

  while ((o = getopt(argc, argv,
		     "f:t:v:h")) != -1) {

    switch(o) {
    case 'f':
      filename = malloc(strlen(optarg)+1);
      memcpy(filename, optarg, strlen(optarg)+1);
      break;
    case 't':
      insert_time = atoi(optarg);
      break;
    case 'v':
      value = atoi(optarg);
    }

  }

  if (!filename || !insert_time || !value)
    return usage();

  struct NGR_metric_t *metric = NGR_open(filename);

  NGR_insert(metric, 0, insert_time, value);

  return 0;
}
