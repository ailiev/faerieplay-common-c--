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
