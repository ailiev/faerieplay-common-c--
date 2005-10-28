/*
 * ** PIR Private Directory Service prototype
 * ** Copyright (C) 2003 Alexander Iliev <iliev@nimbus.dartmouth.edu>
 * **
 * ** This program is free software; you can redistribute it and/or modify
 * ** it under the terms of the GNU General Public License as published by
 * ** the Free Software Foundation; either version 2 of the License, or
 * ** (at your option) any later version.
 * **
 * ** This program is distributed in the hope that it will be useful,
 * ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 * ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * ** GNU General Public License for more details.
 * **
 * ** You should have received a copy of the GNU General Public License
 * ** along with this program; if not, write to the Free Software
 * ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * */

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
