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

