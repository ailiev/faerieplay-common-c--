/* -*- c++ -*- */
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


#include <exception>
#include <list>

#include <errno.h>

#include <common/countptr.hpp>
#include <common/utils.h>
#include <common/exceptions.h>


#ifndef _SOCKET_CLASS_H
#define _SOCKET_CLASS_H


#define THROW_COMM_EX throw comm_exception \
    ( string(__PRETTY_FUNCTION__) + ": " + strerror (errno) )


/// abstract class to represent a socket address
struct SocketAddress {
    // this virtual destructor is just so that this hierarchy becomes virtual
    // and has runtime type information available
    virtual ~SocketAddress() throw () {}
};


/// abstract class to represent a POSIX style socket
class Socket {

 public:

    enum socket_type {
	SERVER,
	CLIENT
    };
    
    /// bind to a given local address
    /// @param local_addr address to bind to. must be of the right type for this
    /// socket
    /// @throw std::bad_cast local_addr is not the right type of Address for
    /// this Socket
    virtual void bind (const SocketAddress & local_addr)
	throw (comm_exception, std::bad_cast) = 0;
    
    

    virtual ~Socket () throw () {}
};


/// abstract class to represent a Datagram socket
class DatagramSocket : public Socket {


public:
    
    /// send a message to the given destination
    virtual void sendto (const ByteBuffer& data, const SocketAddress & dest)
	throw (comm_exception, std::bad_cast) = 0;
    
    /// receive a message
    /// @param o_source fill in with the source of the message, which is newly
    /// allocated
    ///
    /// can only be called after a call to bind()
    virtual ByteBuffer recvfrom (counted_ptr<SocketAddress> & o_source)
	throw (comm_exception) = 0;


};



class StreamSocket : public Socket {

public:

    
    virtual void connect (const SocketAddress & peer_addr)
	throw (comm_exception, std::bad_cast)			= 0;

    /// accept a connection.
    /// @param o_source fill in with the source of the connection, which is
    /// newly allocated
    /// @return a pointer to a newly allocated StreamSocket of the
    /// same type, in client mode
    virtual
    counted_ptr<StreamSocket> accept (counted_ptr<SocketAddress> & o_source)
	throw (comm_exception)					= 0;

    /// send some data
    /// must be connected already
    virtual void send (const ByteBuffer& data)
	throw (comm_exception)					= 0;

    /// receive data.
    /// must be either connected, or have accepted a connection
    virtual ByteBuffer recv ()
	throw (comm_exception)					= 0;
};



//
// a unix-style file descriptor
// this is just to be inherited by DatagramFDSocket etc., and used by
// FDSelector
//

class FileDes {

public:

    /// return whether any data is available to read
    /// @param timeout wait so many seconds if no data is immediateley available
    ///      negative timeout means wait forever
//     virtual bool poll (int timeout)
// 	throw (comm_exception);


protected:
    fd_t _fd;


// friends ...
    friend class FDSelector;
};



/// a datagram socket with a file descriptor behind it
class DatagramFDSocket : public DatagramSocket, public FileDes {
};



/// a stream socket with a file descriptor
class StreamFDSocket : public StreamSocket, public FileDes {
};


/// a class to encapsulate the select() system call
class FDSelector {

public:

    /// manipulate the list of sockets to watch
    void add    (const FileDes & sock);
    void remove (const FileDes & sock);
    void clear  ();

    /// Check whether the given socket has data to be read.
    /// @param sock the socket to check
    /// @return whether the socket has data to be read without blocking.
    /// This works with respect to the last select() call, so a single object of
    /// this class is not thread safe (just use different objects in different
    /// threads if needed)
    bool hasData (const FileDes & sock);
    
    /// block up to 'timeout' seconds waiting for data to become available on
    /// any of the sockets being watched
    /// @param timeout how many seconds to wait. negative if no timeout desired.
    /// @return the number of sockets with data ready
    size_t select (int timeout) throw (comm_exception);


private:
    std::list<fd_t> _fds, _ready_fds;
};


#endif // _SOCKET_CLASS_H
