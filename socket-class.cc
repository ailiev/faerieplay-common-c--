#include <unistd.h>
#include <sys/socket.h>
#include <sys/sccnet.h>
#include <netinet/in.h> /* for htons */

#include <common/utils.h>
#include <common/sccutils.h>
#include <common/consts.h>

#include "socket-class.h"


using namespace std;

#define THROW_COMM_EX(place) throw comm_exception (string (place ": ") + \
						   strerror (errno))




SCCDatagramSocket::SCCDatagramSocket ()
    throw (comm_exception)
    : DatagramSocket ()
{
    initsock();
}


SCCDatagramSocket::SCCDatagramSocket (const SCCSocketAddress & local_addr)
    throw (comm_exception)
    : DatagramSocket (),
      _local_addr (local_addr)
{
    initsock();
    bind (_local_addr);
}



void SCCDatagramSocket::bind (const SocketAddress & local_addr)
    throw (comm_exception, std::bad_cast)
{
    _local_addr = dynamic_cast<const SCCSocketAddress&> (local_addr);
    
    // name the socket
    if ( scc_name_server_sock (_sock, _local_addr.port) != 0 ) {
	THROW_COMM_EX ("SCCDatagramSocket::bind");
    }

}


ByteBuffer SCCDatagramSocket::recv (counted_ptr<SocketAddress> & o_source)
    throw (comm_exception)
{
    // receive into a fixed buffer and return also the address of the sender

    int rc;
    struct sockaddr_scc src_addr;
    socklen_t scclen = sizeof(src_addr);
    
    ByteBuffer answer (new byte[BUFSIZE], BUFSIZE);

    if ((rc = recvfrom
	 (_sock, answer.data(), answer.len(), 0,
	  reinterpret_cast<struct sockaddr *>(&src_addr), &scclen)) < 0)
    {
	THROW_COMM_EX ("SCCDatagramSocket::recv");
    }

    if (static_cast<unsigned>(rc) == answer.len()) {
	// maybe we got a truncation!
	// ...
    }

    counted_ptr<SocketAddress> srcaddr (new SCCSocketAddress (src_addr));
    o_source = srcaddr;
    
    answer.len() = rc;

    return answer;
}



void SCCDatagramSocket::send (const ByteBuffer & data,
			      const SocketAddress & dest)
    throw (comm_exception, std::bad_cast)
{
    int rc;
    struct sockaddr_scc scc_dest;

    const SCCSocketAddress & sccdest =
	dynamic_cast<const SCCSocketAddress &> (dest);

    scc_dest.scc_family = AF_SCC;
    scc_dest.scc_card =   htons(sccdest.cardno);
    scc_dest.scc_port =   htons(sccdest.port);

    if ( (rc = sendto (_sock, data.data(), data.len(),
		       0,
		       reinterpret_cast<struct sockaddr *> (&scc_dest),
		       sizeof (scc_dest)))
	 != static_cast<int>(data.len()) )
    {
	THROW_COMM_EX ("SCCDatagramSocket::send");
    }
}



void SCCDatagramSocket::initsock () throw (comm_exception) {
    // just create the socket
    _sock = socket (PF_SCC, SOCK_DGRAM, 0);

    if (_sock == -1) {
	THROW_COMM_EX ("SCCDatagramSocket::initsock");
    }
}


SCCDatagramSocket::~SCCDatagramSocket () {
    if ( close (_sock) != 0 ) {
	// ??
    }
}
