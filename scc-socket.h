/*
 * Library routines for the Dartmouth CS Private Information Retrieval and
 * Writing (PIR/W) library.
 *
 * Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu> and
 * Sean W. Smith <sws@cs.dartmouth.edu>
 *
 * All rights reserved.
 *
 * This code is released under a BSD license.
 * Please see LICENSE.txt for the full license and disclaimers.
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com)
*/

#include <netinet/in.h>		// for ntohs

#include <sys/sccnet.h>		// for SCCADDR_ANY

#include "socket-class.h"


#ifndef _SCC_SOCKET_H
#define _SCC_SOCKET_H

//
// SCC sockets
//


struct SCCSocketAddress : public SocketAddress {

    SCCSocketAddress (unsigned short port)
	: cardno (SCCADDR_ANY),
	  port   (port)
	{}
    
    SCCSocketAddress (unsigned short cardno, unsigned short port)
	: cardno (cardno),
	  port   (port)
	{}

    SCCSocketAddress (const struct sockaddr_scc& scc_addr)
	: cardno ( ntohs(scc_addr.scc_card) ),
	  port   ( ntohs(scc_addr.scc_port) )
	{}

    void to_scc_addr (struct sockaddr_scc & o_sccaddr) const {
	o_sccaddr.scc_family	= AF_SCC;
	o_sccaddr.scc_card	= htons(cardno);
	o_sccaddr.scc_port	= htons(port);
    }


    SCCSocketAddress () {}

    
    unsigned short cardno, port;
};




//
// this class should as one can imagine be given SCCSocketAddress's in bind()
// and send(), or a bad_cast exception is coming your way!
//
class SCCDatagramSocket : public DatagramFDSocket {

public:
    
    // create an unbound socket, presumably for sending
    SCCDatagramSocket () throw (comm_exception);
    
    // create a bound server socket, or a connected client socket
    explicit SCCDatagramSocket (const SCCSocketAddress & local_addr)
	throw (comm_exception);


    
    virtual void bind (const SocketAddress & local_addr)
	throw (comm_exception, std::bad_cast);
    
    virtual void sendto (const ByteBuffer & data, const SocketAddress & dest)
	throw (comm_exception, std::bad_cast);
    
    virtual ByteBuffer recvfrom (counted_ptr<SocketAddress> & o_source)
	throw (comm_exception);


    virtual ~SCCDatagramSocket () throw();


private:

    void initsock () throw (comm_exception);

    void send_header (const struct sockaddr_scc & scc_dest,
		      size_t len)
	throw (comm_exception);

    // ack's thrown around to keep the sender from flooding the
    // receive
    void send_ack (const struct sockaddr_scc & dest)
	throw (comm_exception);

    void receive_ack (const struct sockaddr_scc & from)
	throw (comm_exception);

    // basic wrappers around the syscalls, to convert the error return
    // values to excaptions
    void wrap_sendto (const void * bytes, ssize_t len,
		      const struct sockaddr_scc & dest)
	throw (comm_exception);

    void wrap_recvfrom (void * into, ssize_t len,
			struct sockaddr_scc * o_src, socklen_t * o_srclen)
	throw (comm_exception);
    

    size_t receive_header () throw (comm_exception);
    
    SCCSocketAddress _local_addr;
};




/// SCC stream (fd) socket
class SCCStreamSocket : public StreamFDSocket {

public:
    
    /// create an unbound/unconnected socket, to be initialized later
    SCCStreamSocket () throw (comm_exception);
    
    /// create a bound server socket or a connected client socket
    /// @param type SERVER or CLIENT
    /// @param addr the local address for a server or the remote
    /// address for a client
    SCCStreamSocket (Socket::socket_type type,
		     const SCCSocketAddress & addr)
	throw (comm_exception);

    
    //
    // the other StreamSocket methods here...
    //

    /// bind to a given local address
    /// @param local_addr address to bind to. must be of the right type for this
    /// socket
    /// @throw std::bad_cast local_addr is not the right type of Address for
    /// this Socket
    void bind (const SocketAddress & local_addr)
	throw (comm_exception, std::bad_cast);

    
    virtual void connect (const SocketAddress & peer_addr)
	throw (comm_exception, std::bad_cast);

    /// accept a connection.
    /// @param o_source fill in with the source of the connection, which is
    /// newly allocated
    /// @return a pointer to a newly allocated StreamSocket of the
    /// same type, in client mode
    counted_ptr<StreamSocket> accept (counted_ptr<SocketAddress> & o_source)
	throw (comm_exception);

    /// send some data
    /// must be connected already
    void send (const ByteBuffer& data)
	throw (comm_exception);

    /// receive data.
    /// must be either connected, or have accepted a connection
    ByteBuffer recv ()
	throw (comm_exception);


    
    virtual ~SCCStreamSocket () throw ();


private:

    void initsock () throw (comm_exception);

    void send_header (size_t len)
	throw (comm_exception);
    
    size_t receive_header () throw (comm_exception);
    

    
    SCCSocketAddress _local_addr;
};

#endif // _SCC_SOCKET_H
