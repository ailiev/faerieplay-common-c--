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

//
// HACK:
// i'll make these objects follow a "protocol" to send packets: in any one
// logical send-recv, one packet will be sent which indicates how large the real
// data is. This will be a 32 bits unsigned int in network byte order.
// Then a packet with the actual data and of known size will be sent.
//
// This is to make up for the inability of the
// underlying socket to tell a user how much data is in an arrived packet.

// WARNING: this means that both sides of a transaction have to follow this
// protocol! Easiest way of course is just to use this object on both sides
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
    socklen_t scclen = sizeof(src_addr);

    size_t size = receive_header ();
    ByteBuffer answer (new byte[size], size);

    if ((rc = recvfrom
	 (_sock, answer.data(), answer.len(), 0,
	  reinterpret_cast<struct sockaddr *>(&src_addr), &scclen)) < 0)
    {
	THROW_COMM_EX ("SCCDatagramSocket::recv");
    }

    if ((unsigned)rc < answer.len()) {
	// missing some data??
	cerr << "SCCDatagramSocket::recv expected " << answer.len() << " bytes."
	     << " Got " << rc << endl;

	answer.len() = rc;
    }

    counted_ptr<SocketAddress> srcaddr (new SCCSocketAddress (src_addr));
    o_source = srcaddr;
    
    return answer;
}


// receive and process a header packet, which for now just contains the size of
// the payload packet
size_t SCCDatagramSocket::receive_header () throw (comm_exception) {

    int rc = 0;
    struct sockaddr_scc src_addr;
    socklen_t scclen = sizeof(src_addr);

    uint32_t size_enc;

    rc = recvfrom
	(_sock, &size_enc, sizeof(size_enc), 0,
	 reinterpret_cast<struct sockaddr *>(&src_addr), &scclen);

    if (unsigned(rc) < sizeof(size_enc)) {
	THROW_COMM_EX ("SCCDatagramSocket::receive_size");
    }

    return ntohl (size_enc);
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

    // send the header packet, with the payload size for now
    send_header(scc_dest, data.len());
    
    if ( (rc = sendto (_sock, data.data(), data.len(),
		       0,
		       reinterpret_cast<struct sockaddr *> (&scc_dest),
		       sizeof (scc_dest)))
	 != static_cast<int>(data.len()) )
    {
	THROW_COMM_EX ("SCCDatagramSocket::send");
    }
}


// Send a header packet, which for now just contains a 32 bit payload size
void SCCDatagramSocket::send_header (const struct sockaddr_scc & scc_dest,
				     size_t len)
    throw (comm_exception)
{
    int rc;
    uint32_t len_enc = htonl (len);
    
    if ( (rc = sendto (_sock, &len_enc, sizeof(len_enc),
		       0,
		       reinterpret_cast<const struct sockaddr *> (&scc_dest),
		       sizeof (scc_dest)))
	 != sizeof(len_enc) )
    {
	THROW_COMM_EX ("SCCDatagramSocket::send_header");
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
