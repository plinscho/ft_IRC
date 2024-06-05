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


//	configurar el socket para que reutilice la dirección y el puerto cuando se cierre.
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error: setsockopt failed" << std::endl;
		exit (EXIT_FAILURE);
	}

// 	The bind() function assigns a IP address to a socket & a port number in the local machine.
//	The data is specified in "sockaddr", where one field is the IP address and the other is the port number.
	if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
		std::cerr << "Failed to bind to port " << _port << "." << std::endl;
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
	std::cout << "Cerrando sockets...\n" << std::endl;
	// Cierra todos los sockets de los clientes
	for (int i = 0; i < MAX_CLIENTS ; i++)
	{
		if (pollVector[i].fd < 0)
			continue ;
		close(pollVector[i].fd);
		handleDisconnection(i);
	}
	// Cierra el socket del servidor
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
	int newClientFd = accept(getSockfd(), (struct sockaddr *)&sockaddr, &addrlen);
	if (newClientFd < 0)
	{
        // No connections available to accept
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
		std::cerr << "Accept error.\nClosing connection." << std::endl;
		close(newClientFd);
	}
	else
	{
		// Ask the user for server password:
		sendMsgFd(newClientFd, "Please enter the server password: ", MSG_DONTWAIT);
		//Read if the client has send a correct password
		char	buffer[64];
		int		bytesRead = read(newClientFd, buffer, 64);
		buffer[bytesRead] = '\0';

		std::string clientPassword(buffer);
		clientPassword.erase(clientPassword.find_last_not_of(" \n\t\r") + 1);
		if (strcmp(clientPassword.c_str(), p_password.c_str()) != 0)
		{
			memset(buffer, 0, sizeof(buffer));
			sendMsgFd(newClientFd, "Password incorrect.\nClosing connection...\n", MSG_DONTWAIT);
			close(newClientFd);
			return ;
		}
		else
			sendMsgFd(newClientFd, "Password correct.\nWelcome\n", MSG_DONTWAIT);
		for (int i = 1 ; i < MAX_CLIENTS ; i++)
		{
			if (pollVector[i].fd == -1)
			{
				pollVector[i].fd = newClientFd;
				pollVector[i].events = POLLIN;
				conectedClients++;
				// Create a new client object and add it to clientVector
				std::string newClientIp = inet_ntoa(sockaddr.sin_addr);
				Client *newClientObj = new Client(newClientFd, newClientIp);
				clientVector.push_back(newClientObj);
				break;
			}
		}
	}
}

/*
	FUNCTION TO PARSE NICKNAME

	int nickOk = 42;
			if (nickOk)
			{
				sendMsgFd(newClientFd, "Please choose a nickname: ", MSG_DONTWAIT);
				memset(buffer, 0, 64);
				int bytesRecv = recvMsgFd(newClientFd, buffer, 64, 0);
				if (bytesRecv < 0)
				{
					close(newClientFd);
					return ;
				}
				buffer[bytesRecv] = '\0';
				std::string receivedData(buffer, bytesRecv);
				std::cout << "Received data: " << receivedData << std::endl;
				std::string tmpNick(buffer, bytesRecv);
				nickOk = checkNick(tmpNick);
				if (nickOk == EMPTY_NICK)
					sendMsgFd(newClientFd, "Nickname cannot be empty!\n", 0);
				else if (nickOk == SIZE_EXCEED)
					sendMsgFd(newClientFd, "Nickname cannot be more than 12 characters!\n", 0);
				else if (nickOk == HAS_SPACE)
					sendMsgFd(newClientFd, "Nickname cannot have spaces!\n", 0);
				else if (nickOk == IS_NOT_ALNUM)
					sendMsgFd(newClientFd, "Nickname can only be alfa numeric chars!\n", 0);
			}
		}


*/

void Server::handleDisconnection(int index)
{
	int disconnectedFd = pollVector[index].fd;
	std::vector<Client *>::iterator clientIterator;

	if (pollVector[index].fd == -1) 
		return ;

	close(pollVector[index].fd);
	pollVector[index].fd = -1;
	for (clientIterator = clientVector.begin() ; 
			clientIterator != clientVector.end() ; clientIterator++)
	{
		if (disconnectedFd == (*clientIterator)->getFd())
		{
			delete *clientIterator;
			clientVector.erase(clientIterator);
			conectedClients--;
			break ;
		}
	}
	std::cout << "Client erased from list."<< std::endl;
}

Client * Server::getClientByFd(int fdMatch)
{
	std::vector<Client *>::iterator tmp;
	for (tmp = clientVector.begin() ; tmp != clientVector.end() ; tmp++)
    {
        if ((*tmp)->getFd() == fdMatch)
            return *tmp;
    }
    return NULL; // Return nullptr if no client matches the fd
}

void Server::handleCmd(const char *buffer, Client *clientObj)
{
	std::string tmpStr(buffer);

	if (tmpStr.find("/nick") == 0)
		sendMsgFd(clientObj->getFd(), "Usage: /nick [newNick]\n", MSG_DONTWAIT);
	
}

void Server::run()
{
	while (true)
	{
		//	pollVector[0] is the listening socket, the rest are the clients
		int pollReturn = poll(&pollVector[0], MAX_CLIENTS, TIMEOUT);
		if (isPowerOn() && pollReturn < 0)
		{
			if (errno == EINTR) // Mira si ha devuelto -1 por ctrl + C de apagado
				break;
			std::cerr << "Error: Poll error" << std::endl;
			exit(EXIT_FAILURE);
		}

		if (pollVector[0].revents & POLLIN)
			handleNewConnection();

		for (int i = 1 ; i < MAX_CLIENTS ; i++)
		{
			if (pollVector[i].fd < 0)
				continue;
			if (pollVector[i].revents & POLLIN) // miramos los eventos de cada cliente[i]
			{
				Client * tmpClient = getClientByFd(pollVector[i].fd); // comparamos los fd del cliente con el almacenado en el servidor.
				if (tmpClient == NULL)
					continue ;
				sendMsgFd(tmpClient->getFd(), tmpClient->getNickname() + ": ", MSG_DONTWAIT);
				int bytesReceived = recv(pollVector[i].fd, buffer, sizeof(buffer), 0);
				buffer[bytesReceived] = '\0'; // NULL terminate string.
				if (bytesReceived <= 0)
					handleDisconnection(i);
				else
				{
					std::cout << tmpClient->getNickname() +"@"+ tmpClient->getAddress() + ": ";
					handleCmd(buffer, tmpClient);	// check if the client has sent a command
					std::cout << buffer;
					tmpClient->sendData(pollVector[i].fd);
				}
			}
		}
	}
}

/*
	OTHER FUNCTIONS THAT ARE NOT METHODS
*/

// Most usually the flag is MSG_DONTWAIT
void	sendMsgFd(int destFd, std::string msg, int flag)
{
  // Check if socket is ready for writing using poll (non-blocking)
  struct pollfd pfd;
  pfd.fd = destFd;
  pfd.events = POLLOUT;
  int pollResult = poll(&pfd, 1, 0);

  if (pollResult == -1) {
    // Handle error on poll
    std::cerr << "Error polling socket for write readiness: " << strerror(errno) << std::endl;
    return;
  } else if (pollResult == 0) {
    // Socket not ready for writing yet (may need to retry later)
    return;
  }

  // Socket is ready for writing, send the message
  ssize_t bytesSent = send(destFd, msg.c_str(), msg.length(), flag);
  if (bytesSent < 0) {
    std::cerr << "Error sending message to client " << destFd << ": " << strerror(errno) << std::endl;
    // Consider closing the connection if error persists
  }
}

// Most usually the flag is MSG_DONTWAIT
int		recvMsgFd(int originFd, char *buffer, size_t maxLen, int flag)
{
	int bytesRecv = recv(originFd, buffer, maxLen - 1, flag);
	buffer[bytesRecv] = '\0';
	return (bytesRecv);
}

int	checkNick(std::string newNick)
{
	if (newNick.empty())
		return EMPTY_NICK;
	if (newNick.size() > 12)
		return SIZE_EXCEED;
	if (newNick.find(' '))
		return HAS_SPACE;
	for (int i = 0 ; newNick.size() ; i++)
	{
		char c = newNick[i];
		if (!isalnum(c))
			return IS_NOT_ALNUM;
	}
	return NICK_OK;
}