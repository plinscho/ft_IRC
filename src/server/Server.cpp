#include "Server.hpp"

Server::Server(int port, char *password) {
	
	_port = port;
	_password = std::string(password);
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd == -1) {
		std::cerr << "Failed to create socket." << std::endl;
		exit(EXIT_FAILURE);
	}

	// This means that calls to send, recv, accept, etc. on this socket will return immediately 
	// rather than waiting for the operation to complete.
	fcntl(_sockfd, F_SETFL, O_NONBLOCK); // Set the socket to non-blocking

	_sockaddr.sin_family = AF_INET;			// IPv4
	_sockaddr.sin_addr.s_addr = INADDR_ANY;	// Accept connections from any IP address
	_sockaddr.sin_port = htons(_port);		// Port to listen on


//	configurar el socket para que reutilice la dirección y el puerto cuando se cierre.
	int opt = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error: setsockopt failed" << std::endl;
		exit (EXIT_FAILURE);
	}

// 	The bind() function assigns a IP address to a socket & a port number in the local machine.
//	The data is specified in "_sockaddr", where one field is the IP address and the other is the port number.
	if (bind(_sockfd, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr)) < 0) {
		std::cerr << "Failed to bind to port " << _port << "." << std::endl;
		exit(EXIT_FAILURE);
	}

	initPoll();
	//  listen() marks the socket referred to by _sockfd as a passive socket, that is, as a socket that will be used to
	//   accept incoming connection requests using accept(2).
	listen(_sockfd, MAX_CLIENTS);
	conectedClients = 0;
	std::cout << "\nServer listening on port " << _port << std::endl;
}

Server::~Server() {
	if (_sockfd != -1) {
		close(_sockfd);
	}
}

// 0. Start the poll vector. 1st vector is the listen one:
void Server::initPoll(void)
{
	struct pollfd _serverPoll;

	_serverPoll.fd = _sockfd;
	_serverPoll.events = POLLIN; // flag set to "there is data to read"
	_serverPoll.revents = 0; // This will be populated in each poll() call.

	_vectorPoll.push_back(_serverPoll); // set listen poll struct into the vector.
}

// 1. While server is listening, keep an eye on new connections
int Server::grabConnection()
{
	sockaddr_in clientAddr;
	socklen_t	clientAddrLen = sizeof(clientAddr);

	int checker = poll(&_vectorPoll[0], 1, 1000);
	if (!checker)
		return (0);
	int newClientFd = accept(_sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (newClientFd < 0)
	{
		// If ctrl + c is pressed, return 0 and shut down server.
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return 0;
		else
			return (quickError("Error.\nAccept() failed in grabConnection()."));
	}
	
	// get the ip in a readable way
	std::string clientIp = inet_ntoa(clientAddr.sin_addr);

	// create a new client and set the fd and ip from earlier
	Client *newClient = new Client(newClientFd, clientIp);

	// give the client a poll struct.
	struct pollfd newClientPoll;
	newClientPoll.fd = newClientFd;
	newClientPoll.events = POLLIN;
	newClientPoll.revents = 0;
	_vectorPoll.push_back(newClientPoll);

	// save client in map, associated with the fd
	_fdToClientMap[newClientFd] = newClient;
	conectedClients++;

	std::cout << "New connection from " << clientIp << " accepted" << std::endl;
	return (0);
}

int		Server::getPort() const
{
	return (this->_port);
}

int		Server::getSockfd() const
{
	return (_sockfd);
}

void Server::handleConns()
{


}


void Server::closeSockets()
{
	std::map<int, Client *>::iterator it;

	std::cout << "Closing server ...\n" << std::endl;

	// Cierra todos los sockets de los clientes
	while (!_fdToClientMap.empty())
	{
		it = _fdToClientMap.begin();
		for (size_t i = 0 ; i < _vectorPoll.size() ; ++i)
		{
			if (_vectorPoll[i].fd == it->first)
			{
				handleDisconnection(i);
				break;
			}
		}
	}
	
	if (_sockfd >= 0)
	{
		close(_sockfd);
		_sockfd = -1;
	}
}


void Server::handleDisconnection(int index)
{
	if (_vectorPoll[index].fd == -1) 
		return ;
	
	std::map<int, Client *>::iterator clientIterator;
	Client *tmpClient = NULL;
	
	int disconnectedFd = _vectorPoll[index].fd;
	clientIterator = _fdToClientMap.find(disconnectedFd);

	if (clientIterator != _fdToClientMap.end())
	{
		tmpClient = clientIterator->second;
		std::cout << tmpClient->getNickname() << " with ip: "
		<< tmpClient->getAddress() << " disconnected from server." << std::endl;
		close(_vectorPoll[index].fd);
		_vectorPoll[index].fd = -1;

		// free memory 
		delete tmpClient;
		tmpClient = NULL;
		_fdToClientMap.erase(clientIterator);
		_vectorPoll.erase(_vectorPoll.begin() + index);	
		conectedClients--;
	}
}

int Server::run()
{
	static int i = 0;
	int ret;
	ret = poll(_vectorPoll.data(), _vectorPoll.size(), 5000);
	if (ret < 0)
		return (quickError("Error.\nPoll() function failed."));
	else if (ret == 0)
		return (quickError("Server timed out.\n"));
	else
	{
		// loop through the poll vector to check events:
		for (size_t i = 0 ; i < _vectorPoll.size() ; ++i)
		{
			if (_vectorPoll[i].fd < 0)
				continue ;
			
			// check the read events
			if (_vectorPoll[i].revents & POLLIN)
			{
				
			}
				// check the write events:
			if (_vectorPoll[i].revents & POLLIN)
			{
				}
				// clear the poll() revents field
			_vectorPoll[i].revents = 0;
		}
	}
	std::cout << "loops: " << ++i << std::endl;
	return (0);
}


