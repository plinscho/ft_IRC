#pragma once
#include <string.h>
#include <cerrno>
#include <cstdlib>      // For exit() and EXIT_FAILURE
#include <iostream>     // For cout
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For read
#include <arpa/inet.h>  // For inet_ntoa

#define MAX_MSG_SIZE 1024

class Client {
  private:
	char				recvBuffer[MAX_MSG_SIZE];
	char				sendBuffer[MAX_MSG_SIZE];
	int 				clientSocketFd;
	std::string			clientIp;
	std::string			nickName;

  public:
    Client(int fd, std::string ip);
    ~Client();
	std::string		getNickname() const ;
	std::string		getAddress() const ;
	int				getFd() const ;
	int				receiveData(int serverFd);
	std::string		getRecvBuffer() const ;
	int				sendData(int serverFd);


};