/*
 * ** PIR Private Directory Service prototype
 * ** Copyright (C) 2002 Alexander Iliev <iliev@nimbus.dartmouth.edu>
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

/*
 * sccutils.h
 * alex iliev, jan 2003
 * headers for some useful routines related to SCC
 */

#include <sys/sccnet.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    // return the socket descriptor, or -1 if failure
    int scc_setup_server_sock (unsigned short port);

    // return -1 on error
    int scc_setup_send (int * o_sock, struct sockaddr_scc *o_servaddr,
			unsigned short cardno, unsigned short port);
    

#ifdef __cplusplus
}
#endif

