#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <../../include/NGR.h>

#include "const-c.inc"

MODULE = NGR		PACKAGE = NGR::C	PREFIX=NGR_

INCLUDE: const-xs.inc

int metric_created(obj);
    	struct NGR_metric_t *	obj
	CODE:
	RETVAL = obj->created;
	OUTPUT:
	RETVAL

int metric_width(obj);
    	struct NGR_metric_t *	obj
	CODE:
	RETVAL = obj->created;
	OUTPUT:
	RETVAL

int metric_resolution(obj);
    	struct NGR_metric_t *	obj
	CODE:
	RETVAL = obj->resolution;
	OUTPUT:
	RETVAL

int metric_version(obj);
    	struct NGR_metric_t *	obj
	CODE:
	RETVAL = obj->version;
	OUTPUT:
	RETVAL

int metric_columns(obj);
    	struct NGR_metric_t *	obj
	CODE:
	RETVAL = obj->columns;
	OUTPUT:
	RETVAL

int range_items(obj);
    	struct NGR_range_t *	obj
	CODE:
	RETVAL = obj->items;
	OUTPUT:
	RETVAL


struct NGR_range_t *
NGR_aggregate(range, interval, data_type)
	struct NGR_range_t *	range
	int	interval
	int	data_type

struct NGR_metric_t *
NGR_create(filename, create_time, resolution, columns)
	char *	filename
	time_t	create_time
	int	resolution
	int	columns

int
NGR_entry(obj, column, idx)
	struct NGR_metric_t *	obj
	int	column
	int	idx

int
NGR_insert(obj, column, timestmp, value)
	struct NGR_metric_t *	obj
	int	column
	time_t	timestmp
	int	value

int
NGR_last_entry_idx(obj, column)
	struct NGR_metric_t *	obj
	int	column

struct NGR_metric_t *
NGR_open(filename)
	char *	filename

struct NGR_range_t *
NGR_range(obj, column, start, end)
	struct NGR_metric_t *	obj
	int	column
	int	start
	int	end

void
NGR_range_free(range)
	struct NGR_range_t *	range

struct NGR_range_t *
NGR_timespan(obj, column, start, end)
	struct NGR_metric_t *	obj
	int	column
	time_t	start
	time_t	end

int
range_entry_value(obj, column, idx)
	struct NGR_range_t *	obj
	int	column
	int	idx
	CODE:
	RETVAL = obj->entry[idx];
	OUTPUT:
	RETVAL

int
range_entry_min(obj, column, idx)
	struct NGR_range_t *	obj
	int	column
	int	idx
	CODE:
	if(obj->agg) {
	  RETVAL = obj->agg[idx].min;
	} else {
	  RETVAL = obj->entry[idx];
	}
	OUTPUT:
	RETVAL

int
range_entry_max(obj, column, idx)
	struct NGR_range_t *	obj
	int	column
	int	idx
	CODE:
	if(obj->agg) {
	  RETVAL = obj->agg[idx].max;
	} else {
	  RETVAL = obj->entry[idx];
	}
	OUTPUT:
	RETVAL

int
range_entry_stddev(obj, column, idx)
	struct NGR_range_t *	obj
	int	column
	int	idx
	CODE:
	if(obj->agg) {
	  RETVAL = obj->agg[idx].stddev;
	} else {
	  RETVAL = 0; // should be undef
	}
	OUTPUT:
	RETVAL

int
range_entry_items_averaged(obj, column, idx)
	struct NGR_range_t *	obj
	int	column
	int	idx
	CODE:
	if(obj->agg) {
	  RETVAL = obj->agg[idx].items_averaged;
	} else {
	  RETVAL = 0; // none
	}
	OUTPUT:
	RETVAL
