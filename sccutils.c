#include "sccutils.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/sccnet.h>
#include <netinet/in.h> /* for htons */


// return the socket descriptor, or -1 if failure
int scc_setup_server_sock (unsigned short port) {

    int s;
    int rc;
    struct sockaddr_scc servaddr;
    
    s = socket (PF_SCC, SOCK_DGRAM, 0);
    if (s < 0) return s;

    servaddr.scc_family = AF_SCC;
    servaddr.scc_card =   htons(SCCADDR_ANY);
    servaddr.scc_port =   htons(port);

    rc = bind (s, (struct sockaddr*) &servaddr, sizeof(servaddr));
    if (rc < 0) {
	close (s); return rc;
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
