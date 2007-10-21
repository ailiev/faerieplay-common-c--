/*
 * Library routines for the Dartmouth CS Private Information Retrieval and
 * Writing (PIR/W) library.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu> and
 * Sean W. Smith <sws@cs.dartmouth.edu>
 *
 * All rights reserved.
 *
 * This code is released under a BSD license.
 * Please see LICENSE.txt for the full license and disclaimers.
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com)
*/


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
