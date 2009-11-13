
#include "NGR.h"
#include <sys/time.h>
#include <stdio.h>


int main() {
  struct timeval how_fast_start;
  struct timeval how_fast_stop;
    
  struct NGR_metric_t *metric= NGR_create("host", "metric_c", time(NULL) - 3600);

  return;
  //  struct NGR_metric_t *metric = NGR_open("host", "metric_year");
  printf("width: %d; created: %d\n", metric->width, metric->created);
  int idx = NGR_last_entry_idx(metric);
  printf("entry: %d\n", NGR_entry(metric,NGR_last_entry_idx(metric)));

  {

    struct NGR_range_t *range = NGR_range(metric, 0, idx);
    gettimeofday(&how_fast_start, NULL);
    int items = range->items;
    int i = 0;
    int total = 0;
    struct NGR_range_t *aggregate = NGR_aggregate(range, 86400,0);
    gettimeofday(&how_fast_stop, NULL);
    printf("Items:%d   Total: %d   Avg: %d\n", range->items, total, total/range->items);
    printf("%d.%d\n", how_fast_start.tv_sec, how_fast_start.tv_usec);
    printf("%d.%d\n", how_fast_stop.tv_sec, how_fast_stop.tv_usec);

    items = aggregate->items;
    i = 0;
    while(items--) {
      //      printf("AVG: %d      MAX: %d      MIN: %d      STDDEV: %d\n", aggregate->agg[i].avg, aggregate->agg[i].max, aggregate->agg[i].min, aggregate->agg[i].stddev);
      i++;
    }

    NGR_range_free(range);
  }
  {
    gettimeofday(&how_fast_start, NULL);
    struct NGR_range_t *range = NGR_timespan(metric, time(NULL)-36000, time(NULL)-35000);
    int items = range->items;
    int i = 0;
    while(items--) {
      int foo = range->entry[i++];
      //      printf("%d\n", range->entry[i++]);
    }
    gettimeofday(&how_fast_stop, NULL);
    printf("%d.%d\n", how_fast_start.tv_sec, how_fast_start.tv_usec);
    printf("%d.%d\n", how_fast_stop.tv_sec, how_fast_stop.tv_usec);
    printf("entries: %d\n", range->items);


    NGR_range_free(range);
  }

}


