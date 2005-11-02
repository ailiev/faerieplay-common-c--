
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>

#include "udp-socket.h"


using namespace std;



#ifdef _TESTING_UDP_SOCK

// for fork:
#include <sys/types.h>
#include <unistd.h>

int main () {

    unsigned short port = 4567;

    pid_t pid = fork();
    if (pid == 0) {
	// child: client
	clog << "Child starting" << endl;
	
	IPSocketAddress send_addr (port, "localhost");
	UDPSocket client;

	sleep (2);
	
	string message = "This is a secret message!";
	client.sendto (ByteBuffer (message), send_addr);

	clog << "client: msg sent" << endl;

	counted_ptr<SocketAddress> dummy;
	ByteBuffer resp = client.recvfrom (dummy);
	cout << "Client received response: "
	     << string (resp.cdata(), resp.len()) << endl;

	exit (0);
    }
    else if (pid > 0) {
	// parent: server

	clog << "Parent starting" << endl;
	
	IPSocketAddress listen_addr (port);
	UDPSocket server (listen_addr);

	counted_ptr<SocketAddress> client_addr;
	
	ByteBuffer msg = server.recvfrom (client_addr);
	clog << "server: msg recvd" << endl;
	
	string msgstr (msg.cdata(), msg.len());

	cout << "Server got message: " << msgstr << endl;

	string response = "And this is the secret response";
	server.sendto (ByteBuffer(response), *client_addr);
    }
    else {
	// error
	perror ("fork");
	exit (EXIT_FAILURE);
    }
}

#endif


//
//
// class UDPSocket
//
//


// create an unbound socket, presumably for sending
UDPSocket::UDPSocket () throw (comm_exception)
{
    initsock ();
}
    
// create a bound server socket, or a connected client socket
UDPSocket::UDPSocket (const IPSocketAddress & local_addr)
    throw (comm_exception)
    : _local_addr (local_addr)
{
    initsock ();
    bind (_local_addr);
}
    

void UDPSocket::bind (const SocketAddress & local_addr)
    throw (comm_exception, std::bad_cast)
{
    _local_addr = dynamic_cast<const IPSocketAddress&> (local_addr);

    struct sockaddr_in name;
    _local_addr.to_in_addr (name);

    if (::bind (_fd, (struct sockaddr *) &name, sizeof (name)) < 0) {
	THROW_COMM_EX;
    }
}

/// send a message to the given destination
void UDPSocket::sendto (const ByteBuffer& data, const SocketAddress & dest)
    throw (comm_exception, std::bad_cast)
{
    struct sockaddr_in sockaddr_dest;
    dynamic_cast<const IPSocketAddress&> (dest).to_in_addr (sockaddr_dest);
    
    ssize_t rc = ::sendto (_fd,
			   data.data(), data.len(),
			   0,
			   (struct sockaddr*) &sockaddr_dest,
			   sizeof(sockaddr_dest));
    if (rc < data.len()) {
	THROW_COMM_EX;
    }
}
    

/// receive a message
/// @param o_source fill in with the source of the message, which is newly
/// allocated
///
/// can only be called after a call to bind()
ByteBuffer UDPSocket::recvfrom (counted_ptr<SocketAddress> & o_source)
    throw (comm_exception)
{
    struct sockaddr_in src;
    socklen_t srclen = sizeof(src);
    ssize_t rc;

    memset (&src, 0, sizeof(src));

    ByteBuffer answer (new byte[1<<15], 1<<15);
    
    // FIXME: should loop this until all the data is picked up!
//    clog << "UDPSocket::recvfrom about to recvfrom()" << endl;
    rc = ::recvfrom (_fd,
		     answer.data(), answer.len(),
		     0,
		     (sockaddr*) &src, &srclen);

    if (rc < 0) {
	THROW_COMM_EX;
    }

    answer.len() = rc;

    counted_ptr<SocketAddress> srcaddr (new IPSocketAddress (src));
    o_source = srcaddr;

    return answer;
}


void UDPSocket::initsock ()
    throw (comm_exception)
{
    // just create the socket
    _fd = socket (PF_INET, SOCK_DGRAM, 0);

    if (_fd == -1) {
	THROW_COMM_EX;
    }
}


UDPSocket::~UDPSocket () throw()
{
    if ( close (_fd) != 0 ) {
	// FIXME: the first "<<" may reset errno, before errmsg gets it
	cerr << "Error closing UDPSocket: " << errmsg << endl;
    }
}
