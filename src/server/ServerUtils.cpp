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

int		Server::getSockfd() const
{
	return (_sockfd);
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
//	std::cout << "sendmsg: " << msg << std::endl;
	if (send(user->getFd(), msg.c_str(), msg.length(), MSG_DONTWAIT) == -1)
		return (-1);
	return (0);
}

int	quickError(std::string msg, int errcode)
{
	std::cerr << msg << std::endl;
	return (errcode);
}

int sendWelcome(int fd)
{
	char buffer[] = "___________________________________________\
	\n\tMIDDLEMAN IRC SERVER\n___________________________________________\
	\nCOMMANDS:\
	\n/login\t\t<password> <nickname>\
	\n/join\t\t<channel>\
	\n/setnick\t<new nick>\t(max 8 characters)\
	\n/setuname\t<new username>\t(max 8 characters)\
	\n/send\t\t<ip addr> | <nickname>\t(send a private msg)\
	\n/help\t\t(print this message again)\n\
	\n";

	if (fd < 0 || send(fd, buffer, sizeof(buffer), MSG_DONTWAIT) == -1)
		return (quickError("Error.\nWecolme message could not be send!", EXIT_FAILURE));
	return (0);
}