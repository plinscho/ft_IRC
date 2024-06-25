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

	// check if listening fd has found a new connection
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
			return (quickError("Error.\nAccept() failed in grabConnection().", EXIT_FAILURE));
	}
	
	// set the client to nonblocking
	if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(newClientFd);
		return (quickError("Error.\nNew connection could not be accepted.", EXIT_SUCCESS));
	}

	// get the ip in a readable way
	std::string clientIp = inet_ntoa(clientAddr.sin_addr);

	// create a new client and set the fd and ip from earlier
	Client *newClient = new Client(newClientFd, clientIp);

	// give the client a poll struct and save it.
	struct pollfd newClientPoll;
	newClientPoll.fd = newClientFd;
	newClientPoll.events = POLLIN;
	newClientPoll.revents = 0;
	_vectorPoll.push_back(newClientPoll);

	// save client in map, associated with the fd
	_fdToClientMap[newClientFd] = newClient;
	if (sendWelcome(newClientFd) != 0)
		handleDisconnection(newClientFd);

	conectedClients++;
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
				handleDisconnection(_vectorPoll[i].fd);
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

std::vector<pollfd>::iterator Server::findPollFd(int fdToMatch)
{
	std::vector<pollfd>::iterator it;

	for (it = _vectorPoll.begin() ; it != _vectorPoll.end() ; ++it)
	{
		if (it->fd == fdToMatch)
			return it;
	}
	return (_vectorPoll.end());
}

void Server::handleDisconnection(int fd)
{
	if (fd == -1) 
		return ;
	
	std::vector<pollfd>::iterator pollIterator;
	std::map<int, Client *>::iterator clientIterator;
	Client *tmpClient = NULL;

	clientIterator = _fdToClientMap.find(fd);
	pollIterator = findPollFd(fd);

	if (clientIterator != _fdToClientMap.end() && pollIterator != _vectorPoll.end())
	{
		tmpClient = clientIterator->second;
		std::cout << tmpClient->getNickname() << " with ip: "
		<< tmpClient->getAddress() << " disconnected from server." << std::endl;
		close(fd);

		// free memory 
		delete tmpClient;
		tmpClient = NULL;

	//	Erasing vectorPoll & Client map with fd	
		_fdToClientMap.erase(clientIterator);
		_vectorPoll.erase(pollIterator);	
		conectedClients--;
	}
}


// Because we can only use poll() 1 time to manage all the revents
// this is the logic I have found the best.
int Server::run()
{
	static int i = 0;
	int ret;

	// call poll() one time and update the _vectorPoll vector.
	ret = poll(_vectorPoll.data(), _vectorPoll.size(), POLL_TIMEOUT);
	if (ret < 0)
	{
		if (errno == EINTR)
			return 0;
		else
			return (quickError("Error.\nPoll() function failed.", EXIT_FAILURE));
	}
	else if (ret == 0)
		return (quickError("Server timed out.\n", EXIT_FAILURE));
	else
	{
		// loop through the poll vector to check events:
		for (size_t i = 0 ; i < _vectorPoll.size() ; ++i)
		{
			if (_vectorPoll[i].fd < 0)
				continue ;

			// check the disconnection events
			else if (_vectorPoll[i].revents & (POLLHUP | POLLERR))
			{
				handleDisconnection(i);
				continue;
			}

			// check the write events:
			if (_vectorPoll[i].revents & POLLIN)
			{
				if (_vectorPoll[i].fd == _sockfd)
					grabConnection();
				else
					receiveData(_vectorPoll[i].fd);
			}

			// clear the poll() revents field
			_vectorPoll[i].revents = 0;
		}
	}
	std::cout << "loops: " << ++i << std::endl;
	return (0);
}

void	Server::receiveData(int fd)
{
	// Vector to save the split cmd
	std::vector<std::string> cmd;
	std::map<int, Client *>::iterator it;

	char buffer[512] = {0};
	Client *tmpClient = NULL;

	// find the client object correspondant from fd
	it = _fdToClientMap.find(fd);
	tmpClient = it->second;


	(void)tmpClient;

	// load the message into a buffer
	size_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead <= 0)
	{
		handleDisconnection(fd);
	}
	else
	{
		// handle receiving message
		buffer[bytesRead] = '\0';
		std::cout << buffer << std::endl;
	}
}


