



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






char * NGR_make_path (char *collection, char *metric) {
  size_t path_len;
  char *path;


  path_len = strlen(collection) + strlen(metric) + 12;  
  path = malloc(path_len);
  snprintf(path, path_len, "data/%s/%s.data", collection, metric);

  return path;
  
}

/* file format is pretty simple
   4 bytes for the width of the fields (32/64)
   4 bytes for the version number of format
   4 bytes for the interval in seconds (interval is not changeable once a file is created
   $width create_timestamp unix_time when the series start
   list of items, indexed of the time-create_time/interval
*/



struct NGR_metric_t * NGR_create(char *collection, char *metric, time_t created_time, int resolution) {
  char *path;
  char buffer[8];
  int   fd, write_len;
  int   size = sizeof(int);
  int version = 1;

  if(!created_time)
    created_time = time(NULL);

  if(!resolution)
    resolution = 60;
  
  path = NGR_make_path(collection, metric);

  fd = open(path, O_CREAT | O_RDWR | O_EXCL, 0755);


  assert(fd != -1);

  
  write_len = write(fd, &size, 4);
  assert(write_len == 4);
  write_len = write(fd, &version, 4);
  assert(write_len == 4);
  write_len = write(fd, &resolution, 4);
  assert(write_len == 4);
  write_len = write(fd, &created_time, size);
  assert(write_len == size);
  close(fd);

  free(path);
  return NGR_open(collection, metric);
}

struct NGR_metric_t * NGR_open(char *collection, char *metric) {
  char *path;
  size_t read_len;
  char width_buf[4];
  struct NGR_metric_t *obj;

  obj = malloc(sizeof(struct NGR_metric_t));


  path = NGR_make_path(collection, metric);

  printf("%s\n", path);

  obj->fd = open(path, O_RDWR);

  obj->base = 4;
  read_len = read(obj->fd, width_buf, 4);
  assert(read_len == 4);
  obj->width = *width_buf;

  obj->base += 4;
  read_len = read(obj->fd, &obj->version, 4);
  assert(read_len == 4);

  obj->base += 4;
  read_len = read(obj->fd, &obj->resolution, 4);
  assert(read_len == 4);
  

  obj->base += obj->width;
  read_len = read(obj->fd, &obj->created, obj->width);
  assert(read_len == obj->width);
  free(path);

  return obj;
}

int NGR_insert (struct NGR_metric_t *obj, time_t timestamp, int value) {
  int  entry, offset, write_len;
  
  if (!timestamp)
    timestamp = time(NULL);

  entry  = ((timestamp - obj->created) / 60);
  offset = (obj->base + ( entry * obj->width ) );
  lseek(obj->fd, offset, SEEK_SET);
  write_len = write(obj->fd, &value, obj->width);
  assert(write_len == obj->width);
  return entry;
}

struct NGR_range_t * NGR_range (struct NGR_metric_t *obj, int start, int end) {
  int items, len, *area, *entry;
  struct NGR_range_t *range;
  struct stat *file;

  range = malloc(sizeof(struct NGR_range_t));
  file = malloc(sizeof(struct stat));
  fstat(obj->fd, file);

  /* if the range goes byond the file, map that as well */
  if ((end * obj->width + obj->base) > file->st_size)
    range->len = (end * obj->width + obj->base);
  else
    range->len = file->st_size;

  free(file);
  range->area = mmap(0, range->len, PROT_READ, MAP_SHARED| MAP_FILE, obj->fd, 0);
  assert(range->area != (void*)-1);
  range->entry = (range->area + obj->base + (obj->width * start));
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
  int offset;
  offset = lseek(obj->fd, 0 - obj->width, SEEK_END);
  if(offset < obj->base)
    return 0;
  return (((int)offset - obj->base) / obj->width);
}

int NGR_entry (struct NGR_metric_t *obj, int idx) {
  char *buf;
  int rv, read_len, offset;
  
  offset = (obj->width + (idx * obj->width));

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

