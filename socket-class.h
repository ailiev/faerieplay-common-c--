/* -*- c++ -*-
 */

#include <exception>

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

    virtual ByteBuffer recv (counted_ptr<SocketAddress> & o_source)
	throw (comm_exception) = 0;


    virtual ~DatagramSocket () {}
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
// this class should as one can imagine be given SCCSocketAddress's in the
// overriden functions
//
class SCCDatagramSocket : public DatagramSocket {

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

    int _sock;
    SCCSocketAddress _local_addr;
};
