/* -*- c++ -*-
 */

#include <exception>
#include <list>

#include <sys/sccnet.h>		// for SCCADDR_ANY
#include <netinet/in.h>		// for ntohs

#include <common/countptr.hpp>

#include <common/exceptions.h>


// abstract class
struct SocketAddress {
    // this virtual destructor is just so that this hierarchy becomes virtual
    // and has runtime type information available
    virtual ~SocketAddress() {}
};


class DatagramSocket {
public:
    
    virtual void bind (const SocketAddress & local_addr)
	throw (comm_exception, std::bad_cast) = 0;
    

    virtual void send (const ByteBuffer& data, const SocketAddress & dest)
	throw (comm_exception, std::bad_cast) = 0;


    /// return whether any data is available to read
    /// @param timeout wait so many seconds if no data is immediateley available
    ///      negative timeout means wait forever
    virtual bool poll (int timeout)
	throw (comm_exception) = 0;


    virtual ByteBuffer recv (counted_ptr<SocketAddress> & o_source)
	throw (comm_exception) = 0;


    virtual ~DatagramSocket () {}
};



//
// a datagram socket with an underlying UNIX fd
//

class DatagramFDSocket : public DatagramSocket {

public:

    // expand the interface a bit, with the select and poll calls
    
    /// return whether any data is available to read
    /// @param timeout wait so many seconds if no data is immediateley available
    ///      negative timeout means wait forever
    virtual bool poll (int timeout)
	throw (comm_exception);


protected:
    int _sock;


// friends ...
    friend class SocketSelector;
};




class SocketSelector {

public:

    /// manipulate the list of sockets to watch
    void add    (const DatagramFDSocket & sock);
    void remove (const DatagramFDSocket & sock);
    void clear  ();

    /// Check whether the given socket has data to be read.
    /// @param sock the socket to check
    /// @return whether the socket has data to be read without blocking.
    /// This works with respect to the last select() call, so a single object of
    /// this class is not thread safe (just use different objects in different
    /// threads if needed)
    bool hasData (const DatagramFDSocket & sock);
    
    /// block up to 'timeout' seconds waiting for data to become available on
    /// any of the sockets being watched
    /// @param timeout how many seconds to wait. negative if no timeout desired.
    /// @return the number of sockets with data ready
    size_t select (int timeout) throw (comm_exception);


private:
    std::list<int> _fds, _ready_fds;
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
    
    // create a bound socket
    explicit SCCDatagramSocket (const SCCSocketAddress & local_addr)
	throw (comm_exception);


    
    virtual void bind (const SocketAddress & local_addr)
	throw (comm_exception, std::bad_cast);
    
    virtual void send (const ByteBuffer & data, const SocketAddress & dest)
	throw (comm_exception, std::bad_cast);
    
    virtual ByteBuffer recv (counted_ptr<SocketAddress> & o_source)
	throw (comm_exception);


    virtual ~SCCDatagramSocket ();


private:

    void initsock () throw (comm_exception);

    SCCSocketAddress _local_addr;
};
