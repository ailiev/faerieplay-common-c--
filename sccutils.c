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
 * sccutils.c
 * alex iliev, jan 2003
 * utility functions for SCC sockets
 */

#include <stdio.h>

#include "sccutils.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/sccnet.h>
#include <netinet/in.h> /* for htons */



int scc_name_server_sock (int s, unsigned short port) {

    int rc;
    struct sockaddr_scc servaddr;
    
    servaddr.scc_family = AF_SCC;
    servaddr.scc_card =   htons(SCCADDR_ANY);
    servaddr.scc_port =   htons(port);

    rc = bind (s, (struct sockaddr*) &servaddr, sizeof(servaddr));
    return rc;
}


// return the socket descriptor, or -1 if failure
int scc_setup_server_sock (unsigned short port) {

    int s;
    int rc;

    unsigned long type = SOCK_DGRAM;
    
//    printf ("Making socket domain %d, type %lu\n", PF_SCC, type);
    
    s = socket (PF_SCC, type, 0);
    if (s < 0) {
	    perror ("scc_setup_server_sock socket()");
	    return s;
    }

    if ( (rc = scc_name_server_sock (s, port)) != 0 ) {
	close(s); return rc;
    }

    return s;
}



int scc_setup_send (int * o_sock, struct sockaddr_scc *o_servaddr,
		    unsigned short cardno, unsigned short port)
{
    if ((*o_sock = socket(PF_SCC, SOCK_DGRAM, 0)) < 0) {
	return *o_sock;
    }

    o_servaddr->scc_family = AF_SCC;
    o_servaddr->scc_card = htons(cardno);
    o_servaddr->scc_port = htons(port);

    return 0;
}
