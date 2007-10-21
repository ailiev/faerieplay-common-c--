/*
 * Library routines for the Dartmouth CS Private Information Retrieval and
 * Writing (PIR/W) library.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu>
 *
 * All rights reserved.
 *
 * This code is released under a BSD license.
 * Please see LICENSE.txt for the full license and disclaimers.
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com)
*/

#include <list>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> /* for htons */

#include <sys/select.h>

#include <common/utils.h>
#include <common/consts.h>

#include "socket-class.h"


using namespace std;





#if 0
//
//
// class FileDes
//
//

bool FileDes::poll (int timeout)
    throw (comm_exception)
{
    struct timeval tv;
    fd_set rfds;
    
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    FD_ZERO (&rfds);
    FD_SET  (_fd, &rfds);

    int rc = ::select (_fd+1, &rfds, NULL, NULL, timeout < 0 ? NULL : &tv);
    if (rc == -1) {
	THROW_COMM_EX;
    }

    return rc > 0;		// rc is > 0 if any of the descriptors have data
				// available
}
#endif // 0



//
//
// class FDSelector
//
//


void FDSelector::add     (const FileDes& sock) {
    _fds.push_front (sock._fd);
}

void FDSelector::remove  (const FileDes& sock) {
    std::remove (_fds.begin(), _fds.end(), sock._fd);
}

void FDSelector::clear () {
    _fds.clear();
}

bool FDSelector::hasData (const FileDes& sock) {
    return
	std::find (_ready_fds.begin(), _ready_fds.end(), sock._fd)
	!=
	_ready_fds.end();
}


size_t FDSelector::select (int timeout) throw (comm_exception) {
    fd_t max_fd = 0;

    struct timeval tv;
    fd_set rfds;

    FD_ZERO (&rfds);
    tv.tv_sec  = timeout;
    tv.tv_usec = 0;

    std::list<fd_t>::iterator i;
    
    for (i = _fds.begin(); i != _fds.end(); i++) {
	max_fd = max (max_fd, *i);
	FD_SET (*i, &rfds);
    }

    int rc = ::select (max_fd + 1, &rfds, NULL, NULL, timeout < 0 ? NULL : &tv);
    if (rc == -1) {
	THROW_COMM_EX;
    }

    // prepare the list of ready fd's
    _ready_fds.clear();
    for (i = _fds.begin(); i != _fds.end(); i++) {
	if ( FD_ISSET (*i, &rfds) ) {
	    _ready_fds.push_front (*i);
	}
    }

    return rc;

}
