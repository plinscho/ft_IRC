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

void Server::closeServer()
{
	std::map<int, Client *>::iterator clientIt;
	std::map<int, Channel *>::iterator channelIt;

	std::cout << "Closing server ...\n" << std::endl;

	// Cierra todos los sockets de los clientes
	while (!_fdToClientMap.empty())
	{
		clientIt = _fdToClientMap.begin();
		for (size_t i = 0 ; i < _vectorPoll.size() ; ++i)
		{
			if (_vectorPoll[i].fd == clientIt->first)
			{
				handleDisconnection(_vectorPoll[i].fd);
				break;
			}
		}
	}
	
	for (channelIt = _channels.begin(); channelIt != _channels.end()  ; ++channelIt)
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
	Client *tmpClient = NULL;

	clientIterator = _fdToClientMap.find(fd);
	pollIterator = findPollFd(fd);

	if (clientIterator != _fdToClientMap.end() && pollIterator != _vectorPoll.end())
	{
		tmpClient = clientIterator->second;
		std::cout << tmpClient->getNickname() << " with ip: "
		<< tmpClient->getAddress() << " disconnected from server." << std::endl;
		close(fd);

		unregisterNickname(tmpClient->getNickname());
		// free memory 
		delete tmpClient;
		tmpClient = NULL;

	//	Erasing vectorPoll & Client map with fd	
		_fdToClientMap.erase(clientIterator);
		_vectorPoll.erase(pollIterator);
		conectedClients--;
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

int	sendMessage(Client *user, const std::string &msg)
{
	if (!user || msg.empty())
		return (-1);
	if (send(user->getFd(), msg.c_str(), msg.length(), MSG_DONTWAIT) == -1)
		return (-1);
	return (1);
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