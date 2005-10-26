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

#include <errno.h>
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

#define THROW_COMM_EX throw comm_exception \
    ( string(__PRETTY_FUNCTION__) + ": " + strerror (errno) )


// empirically, the SCC UDP sockets dont want to send larger packets
// i think 2^5 is the size of the SCC system headers, and whole packets bigger
// than 32K don;t want to be sent
const size_t SCC_MAXPACKET = (32 * 1024) - 32;



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
    if ( scc_name_server_sock (_fd, _local_addr.port) != 0 ) {
	THROW_COMM_EX;
    }

}


ByteBuffer SCCDatagramSocket::recvfrom (counted_ptr<SocketAddress> & o_source)
    throw (comm_exception)
{
    // receive into a buffer and return also the address of the sender

    struct sockaddr_scc src_addr;
    socklen_t scclen = sizeof(src_addr);

    size_t size = receive_header ();
    ByteBuffer answer (new byte[size], size);

    // and now receive the payload, with several packets if needed
    ssize_t remaining;
    unsigned off;
    int thissize;
    for (off = 0, remaining = size; remaining > 0; ) {
	
	thissize = std::min ((size_t)remaining, SCC_MAXPACKET);
	wrap_recvfrom (answer.data() + off, thissize, &src_addr, &scclen);
	
	off		+= SCC_MAXPACKET;
	remaining	-= SCC_MAXPACKET;
	
	// should now send an ack packet to the sender! but not after
	// the last packet
	if (remaining > 0) {
	    send_ack(src_addr);
	}
	
    }
    
    counted_ptr<SocketAddress> srcaddr (new SCCSocketAddress (src_addr));
    o_source = srcaddr;
    
    return answer;
}



void SCCDatagramSocket::sendto (const ByteBuffer & data,
				const SocketAddress & dest)
    throw (comm_exception, std::bad_cast)
{
    struct sockaddr_scc scc_dest;

    const SCCSocketAddress & sccdest =
	dynamic_cast<const SCCSocketAddress &> (dest);

    sccdest.to_scc_addr (scc_dest);

    // send the header packet, with the payload size for now
    send_header(scc_dest, data.len());
    
    // and now send the payload, with several packets if needed
    ssize_t remaining;
    unsigned off;
    size_t thissize;
    for (off = 0, remaining = data.len(); remaining > 0; ) {
	
// 	if (off >= SCC_MAXPACKET) {
// 	    clog << "more iterations of sendto!" << endl;
// 	}
	thissize = std::min ((size_t)remaining, SCC_MAXPACKET);

	wrap_sendto (data.data() + off, thissize, scc_dest);

	off		+= SCC_MAXPACKET;
	remaining	-= SCC_MAXPACKET;

	// wait for the acknowledgement, but only if this is not the
	// last packet
	if (remaining > 0) {
	    clog << "receive_ack()" << endl;
	    receive_ack(scc_dest);
	}

    }
}


// receive and process a header packet, which for now just contains the size of
// the payload packet
size_t SCCDatagramSocket::receive_header () throw (comm_exception) {

    struct sockaddr_scc src_addr;
    socklen_t scclen = sizeof(src_addr);

    uint32_t size_enc;

    wrap_recvfrom (&size_enc, sizeof(size_enc), &src_addr, &scclen);

    return ntohl (size_enc);
}



// Send a header packet, which for now just contains a 32 bit payload size
void SCCDatagramSocket::send_header (const struct sockaddr_scc & scc_dest,
				     size_t len)
    throw (comm_exception)
{
    uint32_t len_enc = htonl (len);
    
    wrap_sendto (&len_enc, sizeof(len_enc), scc_dest);
}


inline void SCCDatagramSocket::send_ack (const struct sockaddr_scc & dest)
    throw (comm_exception)
{
    int ack = 0;
    wrap_sendto (&ack, sizeof(ack), dest);
}


void SCCDatagramSocket::receive_ack (const struct sockaddr_scc & from)
    throw (comm_exception)
{
    int ack;
    struct sockaddr_scc sender;
    socklen_t senderlen = sizeof(sender);
    
    wrap_recvfrom (&ack, sizeof(ack), &sender, &senderlen);

    // TODO: check that 'from' and 'sender' match!
}



void SCCDatagramSocket::wrap_sendto (const void * bytes, ssize_t len,
				     const struct sockaddr_scc & dest)
    throw (comm_exception)
{
    ssize_t rc;

    rc = ::sendto (_fd, bytes, len,
		   0,
		   reinterpret_cast<const struct sockaddr *> (&dest),
		   sizeof (dest));
    if (rc < len) {
	THROW_COMM_EX;
    }
}

void SCCDatagramSocket::wrap_recvfrom (void * into, ssize_t len,
				       struct sockaddr_scc * o_src,
				       socklen_t * o_srclen)
    throw (comm_exception)
{
    ssize_t rc;

    rc = ::recvfrom (_fd, into, len,  0,
		     reinterpret_cast<struct sockaddr *>(o_src), o_srclen);

#ifndef NDEBUG
    if (rc < len) {
	// missing some data??
	cerr << __func__ << ": expected " << len << " bytes."
	     << " Got " << rc << endl;
    }
#endif

    if (rc < 0) {
	THROW_COMM_EX;
    }

}


    
void SCCDatagramSocket::initsock () throw (comm_exception) {
    // just create the socket
    _fd = socket (PF_SCC, SOCK_DGRAM, 0);

    if (_fd == -1) {
	THROW_COMM_EX;
    }
}


SCCDatagramSocket::~SCCDatagramSocket () throw () {
    if ( close (_fd) != 0 ) {
	cerr << "Error closing SCCDatagramSocket: " << errmsg << endl;
    }
}






////////////////////////
//
// class SCCStreamSocket
//
////////////////////////


SCCStreamSocket::SCCStreamSocket () throw (comm_exception)
{
    initsock();
}


// create a bound socket and start it listening
SCCStreamSocket::SCCStreamSocket (Socket::socket_type type,
				  const SCCSocketAddress & addr)
    throw (comm_exception)
{
    initsock();

    switch (type) {
    case SERVER:
	// bind and listen. addr is the local address
	_local_addr = addr;
	bind (_local_addr);
	::listen (_fd, 5);
	break;
    case CLIENT:
	// connect. addr is the remote address
	connect (addr);
	break;
    }
}


SCCStreamSocket::~SCCStreamSocket () throw () {
    if ( close (_fd) != 0 ) {
	cerr << "Error closing SCCStreamSocket: " << errmsg << endl;
    }
}



void SCCStreamSocket::bind (const SocketAddress & local_addr)
    throw (comm_exception, std::bad_cast)
{
    _local_addr = dynamic_cast<const SCCSocketAddress&> (local_addr);
    
    // name the socket
    if ( scc_name_server_sock (_fd, _local_addr.port) != 0 ) {
	THROW_COMM_EX;
    }
}



void SCCStreamSocket::connect (const SocketAddress & dest)
    throw (comm_exception, std::bad_cast)
{
    int rc;

    const SCCSocketAddress & sccdest =
	dynamic_cast<const SCCSocketAddress &> (dest);

    struct sockaddr_scc dest_addr;
    socklen_t scclen = sizeof(dest_addr);

    sccdest.to_scc_addr (dest_addr);


    rc = ::connect (_fd,
		    reinterpret_cast<const struct sockaddr*> (&dest_addr),
		    scclen);

    if (rc != 0) {
	THROW_COMM_EX;
    }
}


counted_ptr<StreamSocket> SCCStreamSocket::accept
(counted_ptr<SocketAddress> & o_source)
    throw (comm_exception)
{
    counted_ptr<SCCStreamSocket> answer (new SCCStreamSocket());

    struct sockaddr_scc src_addr;
    socklen_t scclen = sizeof(src_addr);

    fd_t conn_fd =		// the new socket's fd
	::accept (_fd,
		  reinterpret_cast<struct sockaddr *> (&src_addr), &scclen);
    if (conn_fd < 0) {
	THROW_COMM_EX;
    }

    answer->_fd = conn_fd;

    // up-cast
    return counted_ptr<StreamSocket> (answer);
}


void SCCStreamSocket::send (const ByteBuffer& data)
    throw (comm_exception)
{
    ssize_t rc = 0;

    send_header (data.len());

    size_t remaining =	data.len();
    unsigned off =	0;
    
    while (remaining > 0) {
	if ( (rc = ::send (_fd, data.data()+off, remaining, 0)) < 0 ) {
	    THROW_COMM_EX;
	}
	remaining -= rc;
	off += rc;
    }
}



ByteBuffer SCCStreamSocket::recv ()
    throw (comm_exception)
{
    // receive into a buffer

    int rc = 0;

    size_t size = receive_header ();
    ByteBuffer answer (new byte[size], size);

    // and now receive the payload, with several packets if needed
    size_t remaining =	size;
    unsigned off =	0;
    while (remaining > 0) {
	if ( (rc = ::recv (_fd, answer.data() + off, remaining, 0)) < 0) {
	    THROW_COMM_EX;
	}
	remaining -= rc;
	off += rc;
    }

    return answer;
}


// Send a header packet, which for now just contains a 32 bit payload size
void SCCStreamSocket::send_header (size_t len)
    throw (comm_exception)
{
    int rc;
    uint32_t len_enc = htonl (len);
    
    if ( (rc = ::send (_fd, &len_enc, sizeof(len_enc), 0)) != sizeof(len_enc) )
    {
	THROW_COMM_EX;
    }
}

	

// receive and process a header packet, which for now just contains
// the size of the payload packet
size_t SCCStreamSocket::receive_header () throw (comm_exception) {

    int rc = 0;
    uint32_t size_enc;

    rc = ::recv (_fd, &size_enc, sizeof(size_enc), 0);

    if (rc < (int) sizeof(size_enc)) {
	THROW_COMM_EX;
    }

    return ntohl (size_enc);
}



void SCCStreamSocket::initsock () throw (comm_exception) {
    // just create the socket
    _fd = socket (PF_SCC, SOCK_STREAM, 0);

    if (_fd == -1) {
	THROW_COMM_EX;
    }
}
