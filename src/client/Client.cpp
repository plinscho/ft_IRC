#include <iostream>
#include "Client.hpp"

Client::Client()
{
	_nickName = "RandomUser";
	_userName = "RandomUser";
	_clientSocketFd = -1;
	_clientIp = "0";
	_clientCount = 0;
	_logged = false;
}

Client::Client(int fd, std::string address)
{
	_clientSocketFd = fd;
	_clientIp = address;
	_nickName = "RandomUser";
	_userName = "RandomUser";
	_clientCount = 0;
	_logged = false;
	std::cout << "New connection established with ip: " + _clientIp << std::endl;
	_clientCount++;

}

Client::~Client() 
{
	std::cout << "Client " + getAddress() + " has disconnected" << std::endl;
	close(_clientSocketFd);
	_clientCount--;
}

std::string Client::getNickname() const
{
	return (_nickName);
}

std::string Client::getAddress() const
{
	return (_clientIp);
}

int Client::getFd() const
{
	return (_clientSocketFd);
}

std::string	Client::getRecvBuffer() const 
{
	return (clientBuffer);
}

void	Client::setNickname(std::string newNick)
{

	for (std::vector<std::string>::iterator it = _usedNicks.begin(); it != _usedNicks.end(); ++it) {
		if (*it == newNick) {
				std::cout << "Nick already exist" << std::endl;
				return ;
			}
		}
	this->_nickName = newNick;
	_usedNicks.push_back(_nickName);
}

void	Client::setUserName(std::string newUser)
{
	this->_userName = newUser;
}

void	Client::setLogin(bool option)
{
	this->_logged = option;
}

bool	Client::getLogin(void) const
{
	return (this->_logged);
}

int	Client::getClientCount(void) const
{
	return (this->_clientCount);
}

bool Client::lookNickAlreadyExist(std::string nick)
{
	for (std::vector<std::string>::iterator it = _usedNicks.begin(); it != _usedNicks.end(); ++it)
	{
		if (*it == nick)
		{
			std::cout << "Nick already exist" << std::endl;
			return true;
		}
	}
	return false;
}

