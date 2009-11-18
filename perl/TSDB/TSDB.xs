#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <../../include/TSDB.h>

#include "const-c.inc"

MODULE = TSDB		PACKAGE = TSDB::C	PREFIX=TSDB_

INCLUDE: const-xs.inc


char* metric_name(obj);
    	struct TSDB_metric_t *	obj
	CODE:
	RETVAL = obj->name;
	OUTPUT:
	RETVAL      

HV* metric_meta(obj);
    	struct TSDB_metric_t *	obj
	CODE:
	int i; 
	RETVAL = newHV();
	for(i = 0; i < obj->columns; i++) {
	      SV* flags = newSViv(obj->col_flags[i]);
	      hv_store(RETVAL, (char*) obj->col_names[i], strlen(obj->col_names[i]), flags, 0);
	}	
	OUTPUT:
	RETVAL      

int metric_created(obj);
    	struct TSDB_metric_t *	obj
	CODE:
	RETVAL = obj->created;
	OUTPUT:
	RETVAL

int metric_width(obj);
    	struct TSDB_metric_t *	obj
	CODE:
	RETVAL = obj->created;
	OUTPUT:
	RETVAL

int metric_resolution(obj);
    	struct TSDB_metric_t *	obj
	CODE:
	RETVAL = obj->resolution;
	OUTPUT:
	RETVAL

int metric_version(obj);
    	struct TSDB_metric_t *	obj
	CODE:
	RETVAL = obj->version;
	OUTPUT:
	RETVAL

int metric_columns(obj);
    	struct TSDB_metric_t *	obj
	CODE:
	RETVAL = obj->columns;
	OUTPUT:
	RETVAL

int range_rows(obj);
    	struct TSDB_range_t *	obj
	CODE:
	RETVAL = obj->rows;
	OUTPUT:
	RETVAL


struct TSDB_range_t *
TSDB_aggregate(range, interval, data_type)
	struct TSDB_range_t *	range
	int	interval
	int	data_type

struct TSDB_metric_t *
TSDB_create(filename, create_time, resolution, columns, name, flags, col_names, col_flags)
	char *	filename
	time_t	create_time
	int	resolution
	int	columns
	char *  name
	int     flags
	AV*	col_names
	AV*	col_flags
	CODE:
	int i;
	struct TSDB_create_opts_t *opts = TSDB_create_opts(columns);

	for(i = 0; i < columns; i++) {
	      opts->col_names[i] = SvPVbyte_nolen(*av_fetch(col_names, i, 0));
	      opts->col_flags[i] = SvIV(*av_fetch(col_flags, i, 0));
	}
	opts->filename = filename;
	opts->created_time = create_time;
	opts->resolution = resolution;
	opts->name = name;
	opts->flags = flags;
	RETVAL = TSDB_create(opts);
	TSDB_free_opts(opts);
	OUTPUT:
	RETVAL
	

int
TSDB_cell(obj, row, column)
	struct TSDB_metric_t *	obj
	int	row
	int	column

int
TSDB_insert(obj, column, timestmp, value)
	struct TSDB_metric_t *	obj
	int	column
	time_t	timestmp
	int	value

int
TSDB_last_row_idx(obj, column)
	struct TSDB_metric_t *	obj
	int	column

struct TSDB_metric_t *
TSDB_open(filename)
	char *	filename

struct TSDB_range_t *
TSDB_range(obj, start, end)
	struct TSDB_metric_t *	obj
	int	start
	int	end

void
TSDB_range_free(range)
	struct TSDB_range_t *	range

struct TSDB_range_t *
TSDB_timespan(obj, start, end)
	struct TSDB_metric_t *	obj
	time_t	start
	time_t	end

int
range_row_value(obj, column, idx)
	struct TSDB_range_t *	obj
	int	column
	int	idx
	CODE:
	RETVAL = obj->row[(idx * obj->columns) + column];
	OUTPUT:
	RETVAL

double
range_row_avg(obj, column, idx)
	struct TSDB_range_t *	obj
	int	column
	int	idx
	CODE:
	if(obj->agg) {
	  RETVAL = obj->agg[(idx * obj->columns) + column].avg;
	} else {
	  RETVAL = obj->row[(idx * obj->columns) + column];
	}
	OUTPUT:
	RETVAL

int
range_row_min(obj, column, idx)
	struct TSDB_range_t *	obj
	int	column
	int	idx
	CODE:
	if(obj->agg) {
	  RETVAL = obj->agg[(idx * obj->columns) + column].min;
	} else {
	  RETVAL = obj->row[(idx * obj->columns) + column];
	}
	OUTPUT:
	RETVAL

int
range_row_max(obj, column, idx)
	struct TSDB_range_t *	obj
	int	column
	int	idx
	CODE:
	if(obj->agg) {
	  RETVAL = obj->agg[(idx * obj->columns) + column].max;
	} else {
	  RETVAL = obj->row[(idx * obj->columns) + column];
	}
	OUTPUT:
	RETVAL

double
range_row_stddev(obj, column, idx)
	struct TSDB_range_t *	obj
	int	column
	int	idx
	CODE:
	if(obj->agg) {
	  RETVAL = obj->agg[(idx * obj->columns) + column].stddev;
	} else {
	  RETVAL = 0; // should be undef
	}
	OUTPUT:
	RETVAL

int
range_row_rows_averaged(obj, column, idx)
	struct TSDB_range_t *	obj
	int	column
	int	idx
	CODE:
	if(obj->agg) {
	  RETVAL = obj->agg[(idx * obj->columns) + column].rows_averaged;
	} else {
	  RETVAL = 0; // none
	}
	OUTPUT:
	RETVAL
