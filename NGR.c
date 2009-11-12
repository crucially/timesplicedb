



#include "NGR.h";
#include <stdio.h>;
#include <fcntl.h>;
#include <string.h>;
#include <stdlib.h>;
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>

int main() {
  struct timeval how_fast_start;
  struct timeval how_fast_stop;
    
  struct NGR_metric_t *metric = NGR_open("host", "metric_year");
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

struct NGR_metric_t * NGR_open(char *host, char *metric) {
  char *path;
  size_t read_len, path_len;
  char width_buf[4];
  struct NGR_metric_t *obj;

  obj = malloc(sizeof(struct NGR_metric_t));

  path_len = strlen(host) + strlen(metric) + 12;


  path = malloc(path_len);
  snprintf(path, path_len, "data/%s/%s.data", host, metric);

  obj->fd = open(path, O_RDWR);

  read_len = read(obj->fd, width_buf, 4);
  assert(read_len == 4);
  obj->width = *width_buf;
  
  read_len = read(obj->fd, &obj->created, obj->width);
  assert(read_len == obj->width);

  free(path);

  return obj;
}

struct NGR_range_t * NGR_range (struct NGR_metric_t *obj, int start, int end) {
  int items, len, *area, *entry;
  struct NGR_range_t *range;
  struct stat *file;

  range = malloc(sizeof(struct NGR_range_t));
  file = malloc(sizeof(struct stat));
  fstat(obj->fd, file);

  /* if the range goes byond the file, map that as well */
  if ((end * obj->width + 4 + obj->width) > file->st_size)
    range->len = (end * obj->width + 4 + obj->width);
  else
    range->len = file->st_size;

  free(file);
  range->area = mmap(0, range->len, PROT_READ, MAP_SHARED| MAP_FILE, obj->fd, 0);
  assert(range->area != (void*)-1);
  range->entry = (range->area + 4 + obj->width + (obj->width * start));
  range->items = end - start;
  range->mmap = 1;
  range->agg = 0;
  return range;
}

void NGR_range_free (struct NGR_range_t * range) {

  if (range->mmap == 1)
    munmap(range->area, range->len);
  else
    free(range->area);

  if(range->agg)
    free(range->agg);

  free(range);
}

struct NGR_range_t * NGR_timespan (struct NGR_metric_t *obj, time_t start, time_t end) {
  int start_offset, end_offset;

  start_offset = ((start - obj->created + 60) / 60);
  end_offset = ((end - obj->created + 60) / 60);
  return NGR_range(obj, start_offset, end_offset);
}


int NGR_last_entry_idx (struct NGR_metric_t *obj) {
  off_t offset;
  offset = lseek(obj->fd, 0 - obj->width, SEEK_END);
  return (((int)offset - 4 - obj->width) / obj->width);
}

int NGR_entry (struct NGR_metric_t *obj, int idx) {
  char *buf;
  int rv, read_len, offset;
  
  offset = (4 + obj->width + (idx * obj->width));

  buf = malloc(obj->width);
  assert(sizeof(rv) == obj->width);
  lseek(obj->fd, offset, SEEK_SET);

  read_len = read(obj->fd, buf, obj->width);
  assert(read_len == obj->width);

  memcpy(&rv, buf, obj->width);
  free(buf);
  return rv;
}


struct NGR_range_t * NGR_aggregate (struct NGR_range_t *range, int interval, int data_type) {
  struct NGR_range_t *aggregate;
  int src_items, trg_items, items_seen, curr_item, sum, sum_sqr, min, max;
  

  src_items = range->items;
  max = sum_sqr = sum = curr_item = items_seen = trg_items = 0;
  min = 2147483647; /** broken on 64bit, i know, and I haven't how to deal with signed or unsigned yet probably counters
			are unsigned and gauge signed?**/

  /* figure out how many buckets we need */
  int buckets = (range->items / (interval / 60));
  printf("need %d buckets\n", buckets);
  aggregate = malloc(sizeof(struct NGR_range_t));
  aggregate->area = malloc(sizeof(int) * buckets);
  aggregate->agg = malloc(sizeof(struct NGR_agg_entry_t) * buckets);
  aggregate->mmap = 0;
  aggregate->items = buckets;
  aggregate->entry = aggregate->area;

  while(src_items--) {
    int value;
    if (data_type == NGR_GAUGE || curr_item == 0)
      value = range->entry[curr_item];
    else 
      value = range->entry[curr_item] - range->entry[curr_item-1];
    
    sum += value;
    sum_sqr += value*value;

    if (min > value)
      min = value;
    if (max < value)
      max = value;

    if(items_seen++ == (interval/60)) {
      aggregate->entry[trg_items] = sum / items_seen;
      aggregate->agg[trg_items].max = max;
      aggregate->agg[trg_items].min = min;
      aggregate->agg[trg_items].stddev = ((sum_sqr - (sum * (sum / items_seen)))/(items_seen-1));
      aggregate->agg[trg_items++].avg = sum / items_seen;
      sum_sqr = max = sum = items_seen = 0;
      min = 2147483647;
    }
    curr_item++;
  }
  aggregate->agg[trg_items].avg = sum / items_seen;
  aggregate->agg[trg_items].max = max;
  aggregate->agg[trg_items].min = min;
  aggregate->agg[trg_items].stddev = ((sum_sqr - (sum * (sum / items_seen)))/(items_seen-1));
  aggregate->entry[trg_items++] = sum / items_seen;
  return aggregate;
}

