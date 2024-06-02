#include "Server.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>

Server::Server(int port, char *password) {
	
	_port = port;
	p_password = password;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cerr << "Failed to create socket." << std::endl;
		exit(EXIT_FAILURE);
	}

	// This means that calls to send, recv, accept, etc. on this socket will return immediately 
	// rather than waiting for the operation to complete.
	fcntl(sockfd, F_SETFL, O_NONBLOCK); // Set the socket to non-blocking

	sockaddr.sin_family = AF_INET;			// IPv4
	sockaddr.sin_addr.s_addr = INADDR_ANY;	// Accept connections from any IP address
	sockaddr.sin_port = htons(_port);		// Port to listen on

// 	The bind() function assigns a IP address to a socket & a port number in the local machine.
//	The data is specified in "sockaddr", where one field is the IP address and the other is the port number.
	if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
		std::cerr << "Failed to bind to port" << _port << "." << std::endl;
		exit(EXIT_FAILURE);
	}

	//  listen() marks the socket referred to by sockfd as a passive socket, that is, as a socket that will be used to
	//   accept incoming connection requests using accept(2).
	listen(sockfd, MAX_CLIENTS);
	conectedClients = 1;
	powerOn = true;
	std::cout << "\nServer listening on port " << _port << std::endl;
}

Server::~Server() {
	if (sockfd != -1) {
		close(sockfd);
	}
}

void Server::shutDown()
{
	this->powerOn = false;
}

bool Server::isPowerOn() const
{
	return (powerOn);
}

int		Server::getPort() const
{
	return (_port);
}

int		Server::getSockfd() const
{
	return (sockfd);
}

/*
 The accept() function is used with connection-based socket types (SOCK_STREAM,
 SOCK_SEQPACKET). It extracts the first connection request on the queue of
 pending connections for the listening socket, sockfd, creates a new connected
 socket, and returns a new file descriptor referring to that socket. The newly
 created socket is not in the listening state. The original socket sockfd is
 unaffected by this call. The argument addr is a pointer to a sockaddr
 structure. This structure is filled in with the address of the peer socket, as
 known to the communications layer. The exact format of the address, as returned
 by accept(), depends on the communications domain in which the socket is
 created. The addrlen argument is a value-result argument: it should initially
 contain the size of the structure pointed to by addr, on return it will contain
 the actual size of the peer address.
*/
void Server::handleConns() {

	struct sockaddr_in sockaddr; // This structure will hold the address of the client
	uint16_t addrlen = sizeof(sockaddr); 

	//	Every time a new client connects to the server, the accept() function
	// creates 	a new socket and returns a new file descriptor for that socket.
	//	This new file descriptor is used for communication with that specific
	// client.
	while (1) {
		int connection =
			accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
		if (connection < 0) {
			// throw("in connection");
			std::cout << "aqui peta>????" << std::endl;
		}

		std::string ip = inet_ntoa(sockaddr.sin_addr);

		// Read from the connection
		char buffer[1024];
		ssize_t bytesRead = read(connection, buffer, 100);
		(void)bytesRead;
		std::cout << "nickname@" << ip << ": " << buffer << std::endl;
		memset(buffer, 0, 1024);
		// Send a message to the connection
		// std::string response = "Good talking to you\n";
		// send(connection, response.c_str(), response.size(), 0);
	}
}


void Server::closeSockets()
{
	std::string ip = inet_ntoa(sockaddr.sin_addr);

	// Cierra el socket del servidor
	std::cout << "Cerrando sockets...\n" << std::endl;
	// Cierra todos los sockets de los clientes
	std::cout << "Cerrando socket ip: " << ip << std::endl;
	for (int i = 0; i < MAX_CLIENTS ; i++)
	{
		close(pollVector[i].fd);
	}
	close(sockfd);
}

void Server::initPoll() 
{
    pollVector.clear(); // Clear the vector to make sure it's empty
    pollfd listeningSocket;
    listeningSocket.fd = sockfd; // sockfd is the listening socket
    listeningSocket.events = POLLIN;
    pollVector.push_back(listeningSocket); // Add the listening socket to the vector

    for (int i = 1; i < MAX_CLIENTS; i++) {
        pollfd user;
        user.fd = -1;
        user.events = POLLIN;
        pollVector.push_back(user); // Add the user to the vector
    }
}

void Server::handleNewConnection()
{
	socklen_t addrlen = sizeof(sockaddr);
	int newClient = accept(getSockfd(), (struct sockaddr *)&sockaddr, &addrlen);
	if (newClient < 0)
	{
        // No connections available to accept
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
		std::cerr << "Accept error.\nClosing connection." << std::endl;
		close(newClient);
	}
	else
	{
		for (int i = 1 ; i < MAX_CLIENTS ; i++)
		{
			if (pollVector[i].fd == -1)
			{
				pollVector[i].fd = newClient;
				pollVector[i].events = POLLIN;
				break;
			}
		}
		std::cout << "New connection established with ip: " << inet_ntoa(sockaddr.sin_addr) << std::endl;
	}
}

void Server::run()
{
	while (true)
	{
		//	pollVector[0] is the listening socket, the rest are the clients
		int pollReturn = poll(&pollVector[0], MAX_CLIENTS, TIMEOUT);
		if (pollReturn < 0 && isPowerOn())
		{
			std::cerr << "Error: Poll error" << std::endl;
			exit(EXIT_FAILURE);
		}

		if (pollVector[0].revents & POLLIN)
			handleNewConnection();

		for (int i = 1 ; i < MAX_CLIENTS ; i++)
		{
			if (pollVector[i].fd < 0)
				continue;
			if (pollVector[i].revents & POLLIN)
			{
				char buffer[MAX_MSG_SIZE];
				memset(buffer, 0, MAX_MSG_SIZE);
				int bytesRead = recv(pollVector[i].fd, buffer, MAX_MSG_SIZE, 0);
				if (bytesRead <= 0)
				{
					std::cout << "Client disconnected" << std::endl;
					close(pollVector[i].fd);
					pollVector[i].fd = -1;
				}
				else
				{
					std::cout << "Message from client: " << buffer << std::endl;
					send(pollVector[i].fd, buffer, MAX_MSG_SIZE, 0);
				}
			}
		}
		

	}
}