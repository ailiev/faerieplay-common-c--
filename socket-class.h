/* -*- c++ -*-
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

/* 
 */

#include <exception>
#include <list>

#include <sys/sccnet.h>		// for SCCADDR_ANY
#include <netinet/in.h>		// for ntohs

#include <common/countptr.hpp>

#include <common/exceptions.h>


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




//
// some concrete guys
//

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
