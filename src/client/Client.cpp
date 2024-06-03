#include <iostream>
#include "Client.hpp"

Client::Client(int fd, std::string address) : clientSocketFd(fd), clientIp(address)
{
	std::cout << "New connection established with ip: " + clientIp << std::endl;
	nickName = "newNickname";
}

Client::~Client() 
{
	std::cout << "Client " + getAddress() + " has disconnected" << std::endl;
	close(clientSocketFd);
}

std::string Client::getNickname() const
{
	return (nickName);
}

std::string Client::getAddress() const
{
	return (clientIp);
}

int Client::getFd() const
{
	return (clientSocketFd);
}

std::string	Client::getRecvBuffer() const 
{
	return (std::string(recvBuffer));
}

void	Client::setNickname(std::string newNick)
{
	if (newNick.empty())
		return ;
	this->nickName = newNick;
}

int Client::receiveData(int serverFd)
{
	memset(recvBuffer, 0, sizeof(recvBuffer));
	int bytesReceived = recv(serverFd, recvBuffer, sizeof(recvBuffer), 0);
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
	memset(sendBuffer, 0, sizeof(sendBuffer));
	int bytesSend = send(serverFd, sendBuffer, sizeof(sendBuffer), 0);
    if (bytesSend < 0)
    {
        std::cerr << "Error sending data" << std::endl;
        return -1;
    }
    return bytesSend;
}

