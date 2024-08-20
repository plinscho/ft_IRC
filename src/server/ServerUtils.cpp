#include "Server.hpp"

/*
	OTHER FUNCTIONS THAT ARE NOT METHODS

// Most usually the flag is MSG_DONTWAIT
void	sendMsgFd(int destFd, std::string msg, int flag)
{

}

// Most usually the flag is MSG_DONTWAIT
int		recvMsgFd(int originFd, char *buffer, ssize_t maxLen, int flag)
{

}
*/

int		Server::getPort() const
{
	return (this->_port);
}

std::string Server::getPassword() const
{
	return (this->_password);
}

int		Server::getSockfd() const
{
	return (_sockfd);
}

std::string Server::getServerName(void) const 
{
	return (_serverName);
}

void Server::closeServer()
{
	std::map<int, Client *>::iterator clientIt;
	std::map<std::string, Channel *>::iterator channelIt;

	std::cout << "Closing server ...\n" << std::endl;

	// Cierra todos los sockets de los clientes
	clientIt = _fdToClientMap.begin();
	for (size_t i = 0 ; i < _vectorPoll.size() ; ++i)
	{
		if (_vectorPoll[i].fd == clientIt->first)
		{
			handleDisconnection(_vectorPoll[i].fd);
		}
	}

	for (channelIt = _channels.begin() ; channelIt != _channels.end()  ; ++channelIt)
	{
		delete channelIt->second;
	}
	
	if (_sockfd >= 0)
	{
		close(_sockfd);
		_sockfd = -1;
	}
}

void Server::handleDisconnection(int fd)
{
	if (fd == -1) 
		return ;
	
	std::vector<pollfd>::iterator pollIterator;
	std::map<int, Client *>::iterator clientIterator;
	

	clientIterator = _fdToClientMap.find(fd);
	pollIterator = findPollFd(fd);

	if (clientIterator != _fdToClientMap.end() && pollIterator != _vectorPoll.end())
	{
		Client *tmpClient = clientIterator->second;
		std::cout << "Ip: "
		<< tmpClient->getAddress() << " disconnected from server." << std::endl;
		
		std::vector<std::string>::iterator channelIterator;
		for (channelIterator = tmpClient->channelsJoined.begin() ;
				channelIterator != tmpClient->channelsJoined.end() ; 
				++channelIterator) {
					command.cmdPart(*tmpClient, *this, *channelIterator);
				}

		unregisterNickname(tmpClient->getNickname());

		// free memory 
		delete tmpClient;

	//	Erasing vectorPoll & Client map with fd	
		_fdToClientMap.erase(clientIterator);
		_vectorPoll.erase(pollIterator);
		conectedClients--;
		close(fd);
		pollIterator->fd = -1;
		pollIterator->revents = POLLHUP;
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



int	quickError(std::string msg, int errcode)
{
	std::cerr << msg << std::endl;
	return (errcode);
}

bool	Server::isNicknameInUse(const std::string &nickname) const
{
	std::map<std::string, Client*>::const_iterator it;

	for (it = _nicknameMap.begin() ; it != _nicknameMap.end() ; ++it)
	{
		if (it->first == nickname)
			return (true);
	}
	return (false);
}

void	Server::registerNickname(const std::string &nickname, Client *newUser)
{
	newUser->setNickname(nickname);
	_nicknameMap[nickname] = newUser;
}

void	Server::unregisterNickname(const std::string &nickname)
{
    std::map<std::string, Client*>::iterator it = _nicknameMap.find(nickname);
    if (it != _nicknameMap.end()) {
        _nicknameMap.erase(it);
        std::cout << "Nickname removed: " << nickname << std::endl;
    } else {
        std::cout << "Nickname not found: " << nickname << std::endl;
    }
}