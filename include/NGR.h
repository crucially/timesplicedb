#include <sys/types.h>


#include <sys/uio.h>


#define NGR_GAUGE   0
#define NGR_COUNTER 1

struct NGR_metric_t {
  char       collection[255];
  char       metric[255];
  u_int32_t  width;
  int        fd;
  int        base;
  time_t     created;
  int        resolution;
  int        version;
  int        columns;
};

struct NGR_range_t {
  int *entry;
  void *area;
  int items;
  size_t len;
  int mmap;
  int resolution;
  struct NGR_agg_entry_t *agg;
};

struct NGR_agg_entry_t {
  int avg;
  int max;
  int min;
  int stddev;
};

struct NGR_metric_t * NGR_create(char *filename, time_t create_time, int resolution, int columns);

struct NGR_metric_t * NGR_open(char *filename);


int NGR_last_entry_idx (struct NGR_metric_t *obj, int column);

int NGR_entry (struct NGR_metric_t *obj, int column, int idx);

struct NGR_range_t * NGR_range (struct NGR_metric_t *obj, int column, int start, int end);

void NGR_range_free (struct NGR_range_t *range);

struct NGR_range_t * NGR_timespan (struct NGR_metric_t *obj, int column, time_t start, time_t end);

struct NGR_range_t * NGR_aggregate (struct NGR_range_t *range, int interval, int data_type);

int NGR_insert (struct NGR_metric_t *obj, int column, time_t timestmp, int value);
