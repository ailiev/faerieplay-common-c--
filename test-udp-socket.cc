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

#include <netdb.h>

#include "udp-socket.h"


// for fork:
#include <sys/types.h>
#include <unistd.h>


using namespace std;

int main () {

    unsigned short port = 4567;

    pid_t pid = fork();
    if (pid == 0) {
	// child: client
	clog << "Child starting" << endl;
	
	IPSocketAddress send_addr ("localhost", port);
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
