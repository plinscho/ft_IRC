#include <iostream>
#include "Client.hpp"

Client::Client() : channelsJoined()
{
	_nickName = "RandomUser";
	_userName = "RandomUser";
	_clientSocketFd = -1;
	_clientIp = "0";
	_logged = false;
	_hasNick = false;
	_hasPass = false;
	_hasUser = false;
	changeRevent = false;
}

Client::Client(int fd, std::string address)
{
	_clientSocketFd = fd;
	_clientIp = address;
	_nickName = "";
	_userName = "";
	_hasNick = false;
	_hasPass = false;
	_hasUser = false;
	_logged = false;
	changeRevent = false;
	std::cout << "New connection established with ip: " + _clientIp << std::endl;
}

Client::~Client() {}

std::string Client::getNickname() const
{
	return (_nickName);
}

std::string Client::getUsername() const
{
	return (_userName);
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
	return (_clientBuffer);
}

bool	Client::getLogin(void) const
{
	return (this->_logged);
}

void	Client::setNickname(std::string newNick)
{
	this->_nickName = newNick;
}

void	Client::setUsername(std::string newUser)
{
	this->_userName = newUser;
}

void	Client::setLogin(bool option)
{
	this->_logged = option;
}

void	Client::setBuffer(std::string newBuffer)
{
	if (!newBuffer.empty())
		this->_clientBuffer = newBuffer;
}

bool	Client::getLogStat()
{
	if (getHasNick() && getHasPas() && getHasUser() && getLogin() == false)
		return (true);
	else
		return (false);
}

void	Client::eraseJoinedChannel(std::string channelName)
{
	std::vector<std::string>::iterator it;

	for (it = channelsJoined.begin() ; it != channelsJoined.end() ; ++it)
	{
		if (*it == channelName) {
			channelsJoined.erase(it);
			return;
		}
	}
}

std::string Client::getPrefix() const
{
    return getNickname() + "!" + getUsername() + "@" + getAddress();
}
