#include "comm_types.h"

#include <common/utils-macros.h>

#include <assert.h>


static char * names [] = {
    [STATUS_OK] 	  = "STATUS_OK",
    [STATUS_COMM_FAILURE] = "STATUS_COMM_FAILURE",
    [STATUS_BAD_PARAM] 	  = "STATUS_BAD_PARAM",
    [STATUS_IO_ERROR] 	  = "STATUS_IO_ERROR",
    [STATUS_DIR_ERROR] 	  = "STATUS_DIR_ERROR",
    [STATUS_MISC_ERROR]   = "STATUS_MISC_ERROR"
};
    
const char* host_status_name (host_status_t status)
{
    assert (status >= 0 && status < ARRLEN(names));
    return names[status];
}
