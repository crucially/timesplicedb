



#include "NGR.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>






/* file format is pretty simple
   4 bytes for the width of the fields (32/64)
   4 bytes for the version number of format
   4 bytes for the resolution in seconds (resolution is not changeable once a file is created
   4 bytes for the number of columns
   $width create_timestamp unix_time when the series start
   list of items, indexed of the time-create_time/interval
*/



struct NGR_metric_t * NGR_create(char *filename, time_t created_time, int resolution, int columns) {
  int   fd, write_len;
  int   size = sizeof(int);
  int   version = 1;
  columns = 1; // only support one for now

  if(!created_time)
    created_time = time(NULL);

  if(!resolution)
    resolution = 60;
  
  fd = open(filename, O_CREAT | O_RDWR | O_EXCL, 0755);

  if(fd == -1)
    printf("Cannot open %s (%s)\n", filename, strerror(errno));
  assert(fd != -1);

  
  write_len = write(fd, &size, 4);
  assert(write_len == 4);
  write_len = write(fd, &version, 4);
  assert(write_len == 4);
  write_len = write(fd, &resolution, 4);
  assert(write_len == 4);
  write_len = write(fd, &columns, 4);
  assert(write_len == 4);
  write_len = write(fd, &created_time, size);
  assert(write_len == size);
  close(fd);

  return NGR_open(filename);
}

struct NGR_metric_t * NGR_open(char *filename) {
  size_t read_len;
  char width_buf[4];
  struct NGR_metric_t *obj;

  obj = malloc(sizeof(struct NGR_metric_t));


  obj->fd = open(filename, O_RDWR);

  if(obj->fd == -1)
    printf("Cannot open %s (%s)\n", filename, strerror(errno));
  assert(obj->fd != -1);

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

  obj->base += 4;
  read_len = read(obj->fd, &obj->columns, 4);
  assert(read_len == 4);
  

  obj->base += obj->width;
  read_len = read(obj->fd, &obj->created, obj->width);
  assert(read_len == obj->width);

  assert(obj->width == 4 || obj->width == 8);

  return obj;
}


/* 
   column     which column to write in
   timestamp  which timestamp this is for, defaults to now if not set
   value      value to store
*/

int NGR_insert (struct NGR_metric_t *obj, int column, time_t timestamp, int value) {
  int  entry, offset, write_len;
  
  if (!timestamp)
    timestamp = time(NULL);

  assert (column <= obj->columns - 1);
  assert( timestamp >= obj->created );
  assert (timestamp <= time(NULL) );

  /* We have to transform the timestamp to a entry index and then into a byte offset
     This is rather trivial because we know the value that it was created by, and we can 
     calculate the difference, and then turn that into a byte offset */

  entry  = ((timestamp - obj->created) / obj->resolution);
  offset = (obj->base + ( entry * obj->width ) );
  lseek(obj->fd, offset, SEEK_SET);
  write_len = write(obj->fd, &value, obj->width);
  assert(write_len == obj->width);
  return entry;
}


/*
  column    which column to retrieve
  start     which entry to start with, indexed from 0
  end       what entry to stop at
*/

struct NGR_range_t * NGR_range (struct NGR_metric_t *obj, int column, int start, int end) {
  struct NGR_range_t *range;
  struct stat *file;

  assert (column <= obj->columns - 1);

  range = malloc(sizeof(struct NGR_range_t));
  file = malloc(sizeof(struct stat));
  fstat(obj->fd, file);

  /* if the range goes byond the file, map that as well */
  if ((end * obj->width + obj->base) > file->st_size)
    range->len = (end * obj->width + obj->base);
  else
    range->len = file->st_size;

  /* Ideally we would only map the region we need
     byt I haven't gotten around to write the correct
     page alignment code for that.
     We would need to align to the page, and then recalculate
     the offset in the first page.
  */

  free(file);
  range->area = mmap(0, range->len, PROT_READ, MAP_SHARED| MAP_FILE, obj->fd, 0);
  assert(range->area != (void*)-1);
  range->entry = (range->area + obj->base + (obj->width * start));
  range->items = end - start;
  range->mmap = 1;
  range->agg = 0; 
  range->resolution = obj->resolution;
  return range;
}


/*
  frees a given range
  if it is mmaped then free the are
  aggregates are not mmaped, pure ranges are
*/

void NGR_range_free (struct NGR_range_t * range) {

  if (range->mmap == 1)
    munmap(range->area, range->len);
  else
    free(range->area);

  if(range->agg)
    free(range->agg);

  free(range);
}


/* 
   column    which column
   start     what time this starts from
   stop      and last time we want
*/
struct NGR_range_t * NGR_timespan (struct NGR_metric_t *obj, int column, time_t start, time_t end) {
  int start_offset, end_offset;

  assert (column <= obj->columns - 1);
  /* XXX should check we don't overflow */


  /* this just converts the timestamp into an offset that the range function takes */
  start_offset = ((start - obj->created + obj->resolution) / obj->resolution);
  end_offset = ((end - obj->created + obj->resolution) / obj->resolution);
  return NGR_range(obj, column, start_offset, end_offset);
}


int NGR_last_entry_idx (struct NGR_metric_t *obj, int column) {
  int offset;
  assert (column <= obj->columns - 1);
  offset = lseek(obj->fd, 0 - obj->width, SEEK_END);
  if(offset < obj->base)
    return 0;
  return (((int)offset - obj->base) / obj->width + 1); /** is this really supposed to be please +1 **/
}


int NGR_entry (struct NGR_metric_t *obj, int column, int idx) {
  char *buf;
  int rv, read_len, offset;
  assert (column <= obj->columns - 1);
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
  int buckets = (range->items / (interval / range->resolution));
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

    if(items_seen++ == (interval/range->resolution)) {
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
  /* XXX SSHOULD SET THE RESOLUTION ON AGGREGATE */
  return aggregate;
}

