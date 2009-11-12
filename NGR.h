#include <sys/types.h>


#include <sys/uio.h>


#define NGR_GAUGE   0
#define NGR_COUNTER 1

struct NGR_metric_t {
  char       host[255];
  char       metric[255];
  u_int32_t  width;
  int        fd;
  int        base;
  signed int        created;
};

struct NGR_range_t {
  int *entry;
  void *area;
  int items;
  size_t len;
  int mmap;
  struct NGR_agg_entry_t *agg;
};

struct NGR_agg_entry_t {
  int avg;
  int max;
  int min;
  int stddev;
};

struct NGR_metric_t * NGR_create(char host, char metric, int create_time);

struct NGR_metric_t * NGR_open(char *host, char *metric);


int NGR_last_entry_idx (struct NGR_metric_t *obj);

int NGR_entry (struct NGR_metric_t *obj, int idx);

struct NGR_range_t * NGR_range (struct NGR_metric_t *obj, int start, int end);

void NGR_range_free (struct NGR_range_t *range);

struct NGR_range_t * NGR_timespan (struct NGR_metric_t *obj, time_t start, time_t end);

struct NGR_range_t * NGR_aggregate (struct NGR_range_t *range, int interval, int data_type);
