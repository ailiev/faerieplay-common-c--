// -*- c++ -*-
/*
Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu>

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of the author nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

This product includes cryptographic software written by Eric Young
(eay@cryptsoft.com)

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
