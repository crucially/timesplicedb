//#include "collectd.h"
#include "plugin.h" /* plugin_register_*, plugin_dispatch_values */
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>


#include "NGR.h"

#define PLUGIN_NAME "ngr"

static const char *config_keys[] =
{
   "File",
   "CreatedTime",
   "Resolution"
};

static int config_keys_num  = 3; 
static char   *filename     = NULL;
static time_t created_time  = 0;
static int    resolution    = 0;
extern int    errno;


static int ngr_config (const char *key, const char *value) {
	if (filename)
		free(filename);

	if (strneq(key, "File", 4)) {
		filename = strdup(value);
	} else if (strneq(key, "CreatedTime", 11)) {
		created_time = atoi(value);
	} else if (strneq(key, "Resolution", 10)) {
		resolution   = atoi(value);
	} else {
		return -1;
	}

	return 0;
}


static int ngr_init (void) {
	// NO-OP
	// See note in _shutdown
	DEBUG("ngr plugin: Initialising with store = %s", filename);
}

static struct NGR_metric_t* ngr_create_file (const char *filename, const data_set_t *ds)
{
		struct NGR_metric_t * metric = NGR_create(filename, created_time, resolution, ds->ds_num);
        // for (i = 0; i < ds->ds_num; i++) {
			// set column names
		//}
		return metric;
}


static int ngr_write (const data_set_t *ds, const value_list_t *vl, user_data_t __attribute__((unused)) *user_data) {
	struct stat         statbuf;
	struct flock        fl;
	struct NGR_metric_t *metric;
	int                 i = 0;	
	assert (0 == strcmp (ds->type, vl->type));

	// open the store if necessary
 	if (stat (filename, &statbuf) == -1) {
		if (ENOENT == errno) {
			DEBUG("ngr plugin: No store at %s already - creating it", filename);
			metric = ngr_create_file(filename, ds);
		} else {
       		char errbuf[1024];
            ERROR ("ngr_plugin: stat(%s) failed: %s", filename, sstrerror (errno, errbuf, sizeof (errbuf)));
            return -1;
		}
	} else {
		metric = NGR_open(filename);
	}
	

	if (!metric) { 
		ERROR ("ngr_plugin: Couldn't open %s for some reason", filename);
		return -1; 
	}
	

    memset (&fl, '\0', sizeof (fl));
    fl.l_start  = 0;
    fl.l_len    = 0; // till end of file 
    fl.l_pid    = getpid ();
    fl.l_type   = F_WRLCK;
    fl.l_whence = SEEK_SET;


	// get a lock on the store
	if (0 != fcntl (metric->fd, F_SETLK, &fl)) {
   		char errbuf[1024];
        ERROR ("ngr plugin: flock (%s) failed: %s", filename, sstrerror (errno, errbuf, sizeof (errbuf)));
		NGR_close(metric);
		return -1;
    }

	for (i = 0; i < ds->ds_num; i++) {
		int insert_time = 0 ; // always current time
		// TODO do different things for different types?
		switch (ds->ds[i].type) {
			case DS_TYPE_GAUGE:
				NGR_insert(metric, i, insert_time, vl->values[i].gauge);
				break;
			case DS_TYPE_COUNTER:
				NGR_insert(metric, i, insert_time, vl->values[i].counter);
				break;
			case DS_TYPE_DERIVE:
				NGR_insert(metric, i, insert_time, vl->values[i].derive);
				break;
			case DS_TYPE_ABSOLUTE:
				NGR_insert(metric, i, insert_time, vl->values[i].absolute);
				break;
			default:
				ERROR("ngr plugin: unknown type %s", ds->ds[i].type);
				return -1;
				break;
		}
    }


	// unlock the store explicitly
	if (0 != fcntl (metric->fd, F_SETLK, &fl)) {
        char errbuf[1024];
        ERROR ("ngr plugin: funlock (%s) failed: %s", filename, sstrerror (errno, errbuf, sizeof (errbuf)));
        NGR_close(metric);
        return -1;
    }

	NGR_close(metric);

	return 0;
}

static int ngr_shutdown (void) {
	// NO-OP
	DEBUG("ngr plugin: Shutting down");
	// We lock and unlock all in the _write function
	// The alternative would be to open and close the stor in _init and _shutdown
	// then have a spin lock on a *metric object
	
	// The problem is that we don't know the column names until we start writing values
}

void module_register (void) {       
   plugin_register_config   (PLUGIN_NAME, ngr_config, config_keys, config_keys_num);
   plugin_register_init     (PLUGIN_NAME, ngr_init);
   plugin_register_write    (PLUGIN_NAME, ngr_write, /* user_data = */ NULL);
   plugin_register_shutdown (PLUGIN_NAME, ngr_shutdown); 
}
