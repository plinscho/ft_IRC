#include <iostream>
#include "Client.hpp"

Client::Client()
{
	_nickName = "";
	_userName = "";
	_clientSocketFd = -1;
	_loginAtempts = 0;
	_registered = false;
	_clientIp = "";
	_logged = false;
}

Client::Client(int fd, std::string address)
{
	_clientSocketFd = fd;
	_clientIp = address;
	_loginAtempts = 0;
	_nickName = "";
	_userName = "";
	_logged = false;
	_registered = true;
	std::cout << "New connection established with ip: " + _clientIp << std::endl;

}

Client::~Client() 
{
	std::cout << "Client " + getAddress() + " has disconnected" << std::endl;
	close(_clientSocketFd);
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
	return (std::string(_recvBuffer));
}

void	Client::setNickname(std::string newNick)
{
	this->_nickName = newNick;
}

void	Client::addLoginTry(void)
{
	_loginAtempts += 1;
}

void	Client::setLogin(bool option)
{
	this->_logged = option;
}

bool	Client::getLogin(void) const
{
	return (this->_logged);
}

int Client::receiveData(int serverFd)
{
	memset(_recvBuffer, 0, sizeof(_recvBuffer));
	int bytesReceived = recv(serverFd, _recvBuffer, sizeof(_recvBuffer), 0);
	if (bytesReceived < 0)
	{
		std::cerr << "Error receiving data" << std::endl;
		return (-1);
	}
	else if (bytesReceived == 0)
    {
        std::cerr << "Connection closed by server" << std::endl;
        return (-1);
    }
	return (bytesReceived);
}

int Client::sendData(int serverFd)
{
	memset(_sendBuffer, 0, sizeof(_sendBuffer));
	int bytesSend = send(serverFd, _sendBuffer, sizeof(_sendBuffer), 0);
    if (bytesSend < 0)
    {
        std::cerr << "Error sending data" << std::endl;
        return -1;
    }
    return bytesSend;
}

