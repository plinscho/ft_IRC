#include <iostream>
#include "Client.hpp"

Client::Client(int fd, struct sockaddr_in address) : clientSocketFd(fd), clientAddr(address)
{
	std::cout << "Client connected" << std::endl;
	std::cout << "Client address: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
}

Client::~Client() 
{
	std::cout << "Client disconnected" << std::endl;
	close(clientSocketFd);
}

std::string Client::getAddress() const
{
	std::string ip = inet_ntoa(clientAddr.sin_addr);
	return (ip);
}

int Client::getFd() const
{
	return (clientSocketFd);
}

size_t Client::receiveData(char* buffer, size_t msgLenght)
{
	int bytesReceived = recv(clientSocketFd, buffer, msgLenght, 0);
	if (bytesReceived < 0)
	{
		std::cerr << "Error receiving data" << std::endl;
		return (-1);
	}
	return (0);
}

