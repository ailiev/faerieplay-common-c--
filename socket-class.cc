#include <list>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/sccnet.h>
#include <netinet/in.h> /* for htons */

#include <sys/select.h>

#include <common/utils.h>
#include <common/sccutils.h>
#include <common/consts.h>

#include "socket-class.h"


using namespace std;

#define THROW_COMM_EX(place) throw comm_exception (string (place ": ") + \
						   strerror (errno))


//
//
// class DatagramFDSocket
//
//

bool DatagramFDSocket::poll (int timeout)
    throw (comm_exception)
{
    struct timeval tv;
    fd_set rfds;
    
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    FD_ZERO (&rfds);
    FD_SET  (_sock, &rfds);

    int rc = ::select (_sock+1, &rfds, NULL, NULL, timeout < 0 ? NULL : &tv);
    if (rc == -1) {
	THROW_COMM_EX ("DatagramFDSocket::poll");
    }

    return rc > 0;		// rc is > 0 if any of the descriptors have data
				// available
}




//
//
// class SocketSelector
//
//


void SocketSelector::add     (const DatagramFDSocket& sock) {
    _fds.push_front (sock._sock);
}

void SocketSelector::remove  (const DatagramFDSocket& sock) {
    std::remove (_fds.begin(), _fds.end(), sock._sock);
}

void SocketSelector::clear () {
    _fds.clear();
}

bool SocketSelector::hasData (const DatagramFDSocket& sock) {
    return
	std::find (_ready_fds.begin(), _ready_fds.end(), sock._sock)
	!=
	_ready_fds.end();
}


size_t SocketSelector::select (int timeout) throw (comm_exception) {
    int max_fd = 0;

    struct timeval tv;
    fd_set rfds;

    FD_ZERO (&rfds);
    tv.tv_sec  = timeout;
    tv.tv_usec = 0;

    for (std::list<int>::iterator i = _fds.begin(); i != _fds.end(); i++) {
	max_fd = max (max_fd, *i);
	FD_SET (*i, &rfds);
    }

    int rc = ::select (max_fd + 1, &rfds, NULL, NULL, timeout < 0 ? NULL : &tv);
    if (rc == -1) {
	THROW_COMM_EX ("DatagramFDSocket::select");
    }

    // prepare the list of ready fd's
    _ready_fds.clear();
    for (std::list<int>::iterator i = _fds.begin(); i != _fds.end(); i++) {
	if ( FD_ISSET (*i, &rfds) ) {
	    _ready_fds.push_front (*i);
	}
    }

    return rc;

}

    

	


//
//
// class SCCDatagramSocket
//
//

SCCDatagramSocket::SCCDatagramSocket ()
    throw (comm_exception)
    : DatagramFDSocket ()
{
    initsock();
}


SCCDatagramSocket::SCCDatagramSocket (const SCCSocketAddress & local_addr)
    throw (comm_exception)
    : DatagramFDSocket (),
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
    // receive into a buffer and return also the address of the sender

    int rc = 0;
    struct sockaddr_scc src_addr;
    size_t cap = BUFSIZE;
    size_t size = 0;
    socklen_t scclen = sizeof(src_addr);

    bool have_more;
    
    // FIXME: the overallocation here may be excessive!
    ByteBuffer answer (new byte[cap], cap);

    do {
	if ((rc = recvfrom
	     (_sock, answer.data() + size, answer.len() - size, 0,
	      reinterpret_cast<struct sockaddr *>(&src_addr), &scclen)) < 0)
	{
	    THROW_COMM_EX ("SCCDatagramSocket::recv");
	}

	size += rc;
	
	if (static_cast<unsigned>(rc) == answer.len()) {
	    have_more = true;
	    // enlarge buffer
	    cap *= 2;
	    ByteBuffer b (new byte[cap], cap);
	    memcpy (b.data(), answer.data(), answer.len());
	    answer = b;
	}
	else {
	    have_more = false;
	}
    }
    while (have_more);

    
    counted_ptr<SocketAddress> srcaddr (new SCCSocketAddress (src_addr));
    o_source = srcaddr;
    
    answer.len() = size;

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
