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
	RETVAL = obj->names[0];
	OUTPUT:
	RETVAL      

HV* metric_meta(obj);
    	struct TSDB_metric_t *	obj
	CODE:
	int i; 
	RETVAL = newHV();
	for(i = 1; i < obj->columns + 1; i++) {
	      SV* flags = newSViv(obj->flags[i]);
	      hv_store(RETVAL, (char*) obj->names[i], strlen(obj->names[i]), flags, 0);
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
TSDB_create(filename, create_time, resolution, columns, names, flags)
	char *	filename
	time_t	create_time
	int	resolution
	int	columns
	AV*	names
	AV*	flags
	CODE:
	char **names_x;
	int *flags_x;
	int i;
	names_x = malloc(columns + 1);
	flags_x = malloc(sizeof(int) * (columns + 1));
	for(i = 0; i < columns + 1; i++) {
	      names_x[i] = SvPVbyte_nolen(*av_fetch(names, i, 0));
	      flags_x[i] = SvIV(*av_fetch(flags, i, 0));
	}
	RETVAL = TSDB_create(filename, create_time, resolution, columns, names_x, flags_x);
	free(names_x);
	free(flags_x);
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
