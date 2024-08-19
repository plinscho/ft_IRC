#include "Server.hpp"

Server::Server(int port, char *password) {
	
	_port = port;
	_password = std::string(password);
	_serverName = "Middleman";
}

void	Server::initServer() {
	// Empezamos el servidor
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

// Start the poll vector. 1st vector is the listen one:
void Server::initPoll(void)
{
	struct pollfd _serverPoll;

	_serverPoll.fd = _sockfd;
	_serverPoll.events = POLLIN; // flag set to "there is data to read"
	_serverPoll.revents = 0; // This will be populated in each poll() call.

	_vectorPoll.push_back(_serverPoll); // set listen poll struct into the vector.
}

// 
int Server::grabConnection()
{
	sockaddr_in clientAddr;
	socklen_t	clientAddrLen = sizeof(clientAddr);

	// set if listening fd has found a new connection
	int newClientFd = accept(_sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (newClientFd < 0)
	{
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
	conectedClients++;
	return (0);
}

int Server::updatePoll(void)
{

	int ret;
	// call poll() one time and update the _vectorPoll vector.
	ret = poll(_vectorPoll.data(), _vectorPoll.size(), POLL_TIMEOUT);
	if (ret < 0)
	{
		if (errno == EINTR)
			return (0);
		else
			return (quickError("Error.\nPoll() function failed.", EXIT_FAILURE));
	}
	else if (ret == 0)
		return (quickError("Server timed out.\n", EXIT_FAILURE));
	return (0);
}


// Because we can only use poll() 1 time to manage all the revents
int Server::run()
{
	updatePoll();

	// loop through the poll vector to set events:
	for (size_t i = 0; i < _vectorPoll.size(); ++i)
	{
		if (_vectorPoll[i].fd < 0)
			continue;

		if (_vectorPoll[i].revents & POLLIN)
		{
			if (_vectorPoll[i].fd == _sockfd)
				grabConnection();
			else
				receiveData(_vectorPoll[i]);
		}
		if (_vectorPoll[i].revents & POLLOUT){
				sendData(_vectorPoll[i]);
		}
		else if (_vectorPoll[i].revents & (POLLHUP | POLLERR)) { 
			handleDisconnection(i);
		}
	}
	return (0);
}

void Server::checkBytesRead(int bytesRead, int fd)
{
	if (bytesRead == 0){
		std::cerr <<  __LINE__ << std::endl;
		return (handleDisconnection(fd));
	} else if (bytesRead < 0) {
		std::cerr << "Error in recv function.\nDisconnecting. " << __LINE__ <<std::endl; 
		std::cerr <<  __LINE__ << std::endl;
		return (handleDisconnection(fd));
	}
}

void	Server::receiveData(pollfd &pollStruct)
{
	int	fd = pollStruct.fd;
	char buffer[1024] = {0};

	// load the message into a buffer
	ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
	checkBytesRead(bytesRead, fd);
	buffer[bytesRead] = '\0';

	// find the client object correspondant from fd
	std::map<int, Client *>::iterator it = _fdToClientMap.find(fd);
	if (it == _fdToClientMap.end())	{
		std::cout << "Client not found!" << std::endl;
		//throw "FD NOT FOUND";
		return ;
	}

	// Load client buffer
	std::string tmp = buffer;
	Client *tmpClient = it->second;
	tmpClient->setBuffer(tmpClient->getRecvBuffer().append(tmp));

	// Check if a command is completed
	std::vector<std::string> cmd;
	std::string line = tmpClient->getRecvBuffer();
	cmd = strTool.stringSplit(line, "\r\n");
	if (!cmd.empty())
	{
		std::cout << "UPDATING REVENTS TO POLLOUT! " << std::endl;
		pollStruct.revents = POLLOUT;
		tmpClient->changeRevent = true;
	}
}

void	Server::sendData(pollfd &pollStruct)
{
	int fd = pollStruct.fd;
	std::map<int, Client*>::iterator it = _fdToClientMap.find(fd);

	// Client found in map
	if (it == _fdToClientMap.end())
	{
		std::cerr << "Client not found!" << std::endl; 
		return ;
	}

	// debg info:
	std::cout << "SendData, CMD recived:\n" << it->second->getRecvBuffer() << "**End of CLientbuffer.**" << std::endl;

	// return 1 if QUIT is send from client. Do not continue process.
	if (command.execute(*(it->second), *this)) return ;

	if (it->second->changeRevent == true){
		it->second->clearBuffer();
		it->second->changeRevent = false;
	}
	if (it->second->getLogStat()){
		it->second->setLogin(true);
		message.sendWelcome(*it->second);
		message.sendMessage(*it->second, ":Middleman 005 " + it->second->getNickname() + " NETWORK=Middleman\r\n");
	}
	pollStruct.revents = POLLIN;
}

Client*		Server::getClientByName(std::string &userNick)
{
	std::map<std::string, Client *>::iterator it;

	it = _nicknameMap.find(userNick);
	if (!it->second)
	{
		std::string response;
		response = "Nickname " + userNick + " was not found!\n";
		quickError(response, 0);
		return (NULL);
	}
	return (it->second);
}

Channel*	Server::getChannelByName(std::string channelName)
{
	std::map<std::string, Channel*>::iterator it;
	
	if (channelName.empty())
		return (NULL);
	for (it = _channels.begin() ; it != _channels.end() ; ++it)
	{
		if (channelName == it->second->getChannelName())
			return (it->second);
	}
	return (NULL);
}

bool	Server::channelExists(std::string channelName)
{
	std::map<std::string, Channel*>::iterator it;

	for (it = _channels.begin() ; it != _channels.end() ; ++it)
	{
		if (channelName == it->second->getChannelName())
			return (true);
	}
	return (false);
}
