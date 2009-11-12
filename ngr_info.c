
#include "NGR.h"
#include <sys/time.h>
#include <stdio.h>;
#include <time.h>;

int main() {
  struct NGR_metric_t *metric = NGR_open("foo", "data1");
  time_t last_entry = (metric->created + (NGR_last_entry_idx(metric) * metric->resolution));

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
