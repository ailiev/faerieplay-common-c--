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
 * sccutils.h
 * alex iliev, jan 2003
 * headers for some useful routines related to SCC
 */

#include <sys/sccnet.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    // name an already created socket
    int scc_name_server_sock (int s, unsigned short port);
    
    // return the socket descriptor, or -1 if failure
    int scc_setup_server_sock (unsigned short port);

    // return -1 on error
    int scc_setup_send (int * o_sock, struct sockaddr_scc *o_servaddr,
			unsigned short cardno, unsigned short port);
    

#ifdef __cplusplus
}
#endif

