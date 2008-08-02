// -*- c++ -*-
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

#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>		// for gethostbyname

#include "socket-class.h"



#ifndef _UDP_SOCKET_H
#define _UDP_SOCKET_H


struct IPSocketAddress : public SocketAddress {

    IPSocketAddress (u_int16_t port)
	: port	    (htons(port)),
	  ip_addr   (INADDR_ANY)
	
	{}
    
    IPSocketAddress (std::string host,
		     u_int16_t port)
	: port     (htons(port)),
	  hostname (host)
	{
	    struct hostent * hent = gethostbyname (hostname.c_str());
	    if (hent == NULL) {
		throw comm_exception ("Failed to lookup host " + hostname +
				      ": " + hstrerror (h_errno));
	    }

	    // just get the first IP
	    // it should be in network order already
	    memcpy (&ip_addr, hent->h_addr_list[0], sizeof(ip_addr));
	}

    IPSocketAddress (const sockaddr_in & in_addr)
	: port	    (in_addr.sin_port),
	  ip_addr   (in_addr.sin_addr.s_addr)
	{}

    IPSocketAddress ()
	: port (0),
	  ip_addr (0)
	{}

    std::string description () {
        return hostname + ":" + itoa(ntohs(port));
    }

    void to_in_addr (struct sockaddr_in & o_in_addr) const {
	o_in_addr.sin_family	    = AF_INET;
	o_in_addr.sin_addr.s_addr   = ip_addr;
	o_in_addr.sin_port	    = port;
    }


    ~IPSocketAddress () throw () {}

    
    // these two kept in network byte order
    u_int16_t	port;
    u_int32_t	ip_addr;

    std::string hostname;
    
};



class UDPSocket : public DatagramFDSocket {


public:

    // create an unbound socket, presumably for sending
    UDPSocket () throw (comm_exception);
    
    // create a bound server socket, or a connected client socket
    explicit UDPSocket (const IPSocketAddress & local_addr)
	throw (comm_exception);
    

    virtual void bind (const SocketAddress & local_addr)
	throw (comm_exception, std::bad_cast);

    /// send a message to the given destination
    virtual void sendto (const ByteBuffer& data, const SocketAddress & dest)
	throw (comm_exception, std::bad_cast);
    
    /// receive a message
    /// @param o_source fill in with the source of the message, which is newly
    /// allocated
    ///
    /// can only be called after a call to bind()
    virtual ByteBuffer recvfrom (counted_ptr<SocketAddress> & o_source)
	throw (comm_exception);

    virtual ~UDPSocket () throw();
    

private:

    void initsock () throw (comm_exception);
    
    IPSocketAddress _local_addr;
};





#endif // _UDP_SOCKET_H
